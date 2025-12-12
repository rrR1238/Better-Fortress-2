//====== Copyright Custom Fortress 2, All rights reserved. =================
//
// Custom Item Schema Manager Implementation
//
//=============================================================================

#include "cbase.h"
#include "cf_custom_item_schema.h"
#include "cf_workshop_manager.h"
#include "econ_item_system.h"
#include "econ_item_schema.h"
#include "game_item_schema.h"
#include "filesystem.h"
#include "tier1/KeyValues.h"
#include "tier1/utlbuffer.h"
#include "tier2/fileutils.h"

#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "steam/steam_api.h"
#else
#include "tf_player.h"
#endif

#include "econ_item_inventory.h"

#ifdef CLIENT_DLL
// External function from tf_item_inventory.cpp
extern void ReloadClientItemSchema();
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Global instance
static CCFCustomItemSchemaManager g_CFCustomItemSchemaManager;

// Global accessor
CCFCustomItemSchemaManager* CFCustomItemSchema()
{
	return &g_CFCustomItemSchemaManager;
}

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CCFCustomItemSchemaManager::CCFCustomItemSchemaManager()
	: m_mapCustomItems(DefLessFunc(PublishedFileId_t))
	, m_mapDefToWorkshop(DefLessFunc(item_definition_index_t))
	, m_nNextDefIndex(CF_CUSTOM_ITEM_DEF_START)
	, m_bInitialized(false)
{
}

//-----------------------------------------------------------------------------
// CAutoGameSystem
//-----------------------------------------------------------------------------
bool CCFCustomItemSchemaManager::Init()
{
	CFWorkshopMsg("Custom Item Schema Manager: Init\n");
	m_bInitialized = true;
	return true;
}

void CCFCustomItemSchemaManager::PostInit()
{
	CFWorkshopMsg("Custom Item Schema Manager: PostInit\n");
	
	// Don't auto-load schemas yet - wait for workshop items to be subscribed
}

void CCFCustomItemSchemaManager::Shutdown()
{
	CFWorkshopMsg("Custom Item Schema Manager: Shutdown\n");
	ClearAllCustomSchemas();
	m_bInitialized = false;
}

void CCFCustomItemSchemaManager::LevelInitPreEntity()
{
	// Store current map name
#ifdef CLIENT_DLL
	// On client, use engine->GetLevelName() which returns "maps/mapname.bsp"
	const char* pszLevelName = engine->GetLevelName();
	if (pszLevelName && pszLevelName[0])
	{
		char szMapName[MAX_PATH];
		V_FileBase(pszLevelName, szMapName, sizeof(szMapName));
		m_strCurrentMap = szMapName;
	}
#else
	// On server, use gpGlobals->mapname
	if (gpGlobals->mapname != NULL_STRING)
	{
		m_strCurrentMap = STRING(gpGlobals->mapname);
	}
#endif
	
	CFWorkshopMsg("Custom Item Schema: Level changed to %s\n", m_strCurrentMap.Get());
	
	// Load schemas for this map
	LoadSchemasForCurrentMap();
	
	// Note: CreateInventoryItemsForCustomSchema() is called automatically
	// after schema reload in LoadSchemasForCurrentMap()
}

void CCFCustomItemSchemaManager::LevelShutdownPreEntity()
{
	// Unload map-specific schemas
	CFWorkshopMsg("Custom Item Schema: Unloading map-specific schemas for %s\n", m_strCurrentMap.Get());
	
	// Clear all schemas - they'll be reloaded on next map
	ClearAllCustomSchemas();
	m_strCurrentMap = "";
}

//-----------------------------------------------------------------------------
// Schema management
//-----------------------------------------------------------------------------
bool CCFCustomItemSchemaManager::LoadCustomItemSchema(const char* pszSchemaFile, PublishedFileId_t workshopID, const char* pszMapFilter)
{
	if (!pszSchemaFile || !pszSchemaFile[0])
	{
		CFWorkshopWarning("LoadCustomItemSchema: Invalid schema file path\n");
		return false;
	}
	
	// Workshop ID can be 0 for loose files
	bool bIsLooseFile = (workshopID == 0);
	
	if (!bIsLooseFile)
	{
		// Check if already loaded
		int idx = m_mapCustomItems.Find(workshopID);
		if (idx != m_mapCustomItems.InvalidIndex())
		{
			CFWorkshopMsg("Custom schema for workshop item %llu already loaded\n", workshopID);
			return true;
		}
	}
	
	// Load the KeyValues file
	KeyValues* pKV = new KeyValues("items_game");
	CFWorkshopMsg("Attempting to load custom schema from: %s\n", pszSchemaFile);
	
	if (!pKV->LoadFromFile(filesystem, pszSchemaFile, "GAME"))
	{
		CFWorkshopWarning("Failed to load custom schema file: %s\n", pszSchemaFile);
		pKV->deleteThis();
		return false;
	}
	
	CFWorkshopMsg("Successfully loaded KeyValues from: %s\n", pszSchemaFile);
	
	// Find the items section
	KeyValues* pItems = pKV->FindKey("items");
	if (!pItems)
	{
		CFWorkshopWarning("Custom schema file missing 'items' section: %s\n", pszSchemaFile);
		pKV->deleteThis();
		return false;
	}
	
	// Create entry
	CFCustomItemEntry_t* pEntry = new CFCustomItemEntry_t();
	pEntry->m_nWorkshopID = workshopID;
	pEntry->m_strSchemaFile = pszSchemaFile;
	pEntry->m_bLoaded = false;
	pEntry->m_bEnabled = true;
	pEntry->m_bFromLooseFile = bIsLooseFile;
	if (pszMapFilter)
	{
		pEntry->m_strMapName = pszMapFilter;
	}
	
	// Store the KeyValues for later merging
	pEntry->m_pKeyValues = pKV; // Don't delete, we need it
	
	// Parse each item definition
	bool bAnyLoaded = false;
	for (KeyValues* pItem = pItems->GetFirstSubKey(); pItem; pItem = pItem->GetNextKey())
	{
		// Allocate a def index
		item_definition_index_t defIndex = AllocateDefIndex();
		if (defIndex == INVALID_ITEM_DEF_INDEX)
		{
			CFWorkshopWarning("Failed to allocate def index for custom item\n");
			continue;
		}
		
		// Parse and integrate the item
		if (ParseCustomItemDefinition(pItem, workshopID, defIndex))
		{
			pEntry->m_nDefIndex = defIndex;
			bAnyLoaded = true;
			CFWorkshopMsg("Loaded custom item '%s' with def index %d\n", pItem->GetName(), defIndex);
		}
		else
		{
			ReleaseDefIndex(defIndex);
			CFWorkshopWarning("Failed to parse custom item '%s'\n", pItem->GetName());
		}
	}
	
	if (bAnyLoaded)
	{
		pEntry->m_bLoaded = true;
		
		// Store in appropriate collection
		if (bIsLooseFile)
		{
			m_vecLooseSchemas.AddToTail(pEntry);
			CFWorkshopMsg("Successfully loaded loose custom schema from %s\n", pszSchemaFile);
		}
		else
		{
			m_mapCustomItems.Insert(workshopID, pEntry);
			CFWorkshopMsg("Successfully loaded custom schema for workshop item %llu\n", workshopID);
		}
		return true;
	}
	else
	{
		pKV->deleteThis();
		delete pEntry;
		CFWorkshopWarning("No items loaded from custom schema: %s\n", pszSchemaFile);
		return false;
	}
}

bool CCFCustomItemSchemaManager::ReloadAllCustomSchemas()
{
	CFWorkshopMsg("Reloading all custom item schemas...\n");
	
	// Clear all schemas
	ClearAllCustomSchemas();
	
	// Scan loose files first
	ScanMapsFolder();
	
	// Also check all subscribed items for new schemas
	CUtlVector<PublishedFileId_t> vecAllWorkshopIDs;
	uint32 nSubscribed = CFWorkshop()->GetSubscribedItemCount();
	for (uint32 i = 0; i < nSubscribed; i++)
	{
		PublishedFileId_t fileID = CFWorkshop()->GetSubscribedItem(i);
		if (vecAllWorkshopIDs.Find(fileID) == vecAllWorkshopIDs.InvalidIndex())
		{
			vecAllWorkshopIDs.AddToTail(fileID);
		}
	}
	
	CFWorkshopMsg("Checking %d workshop items for custom schemas...\n", vecAllWorkshopIDs.Count());
	
	// Try to load schema for each item
	int nSuccess = 0;
	FOR_EACH_VEC(vecAllWorkshopIDs, i)
	{
		// Find the workshop item
		CCFWorkshopItem* pItem = CFWorkshop()->GetItem(vecAllWorkshopIDs[i]);
		CFWorkshopMsg("Checking workshop item %llu - pItem=%p\n", vecAllWorkshopIDs[i], pItem);
		if (pItem)
		{
			// Try to register (will check if item has install path)
			if (RegisterWorkshopItem(pItem))
			{
				nSuccess++;
			}
		}
	}
	
	CFWorkshopMsg("Reloaded %d/%d workshop schemas + %d loose schemas\n", nSuccess, vecAllWorkshopIDs.Count(), m_vecLooseSchemas.Count());
	
	// Always write merged schema file (even if empty, to clear it)
	WriteMergedCustomSchema();
	
	// Refresh inventory
	CreateInventoryItemsForCustomSchema();
	
	// Return true if we loaded anything (workshop or loose files)
	return (nSuccess > 0 || m_vecLooseSchemas.Count() > 0);
}

void CCFCustomItemSchemaManager::ClearAllCustomSchemas()
{
	CFWorkshopMsg("Clearing all custom item schemas...\n");
	
	// Remove workshop item schemas
	FOR_EACH_MAP_FAST(m_mapCustomItems, i)
	{
		CFCustomItemEntry_t* pEntry = m_mapCustomItems[i];
		if (pEntry->m_nDefIndex != INVALID_ITEM_DEF_INDEX)
		{
			RemoveCustomDefinitionFromSchema(pEntry->m_nDefIndex);
			ReleaseDefIndex(pEntry->m_nDefIndex);
		}
		delete pEntry;
	}
	
	// Remove loose file schemas
	FOR_EACH_VEC(m_vecLooseSchemas, i)
	{
		CFCustomItemEntry_t* pEntry = m_vecLooseSchemas[i];
		if (pEntry->m_nDefIndex != INVALID_ITEM_DEF_INDEX)
		{
			RemoveCustomDefinitionFromSchema(pEntry->m_nDefIndex);
			ReleaseDefIndex(pEntry->m_nDefIndex);
		}
		delete pEntry;
	}
	
	m_mapCustomItems.Purge();
	m_vecLooseSchemas.Purge();
	m_mapDefToWorkshop.Purge();
	m_nNextDefIndex = CF_CUSTOM_ITEM_DEF_START;
}

void CCFCustomItemSchemaManager::WriteMergedCustomSchema()
{
	CFWorkshopMsg("Writing merged workshop schema to items_workshop.txt...\n");
	
	if (!g_pFullFileSystem)
	{
		CFWorkshopWarning("File system not available, cannot write merged schema\n");
		return;
	}
	
	// Write to items_workshop.txt instead of overwriting items_custom.txt
	char szPath[MAX_PATH];
	V_snprintf(szPath, sizeof(szPath), "scripts/items/items_workshop.txt");
	
	// Merge all custom item definitions
	int nMerged = 0;
	CUtlBuffer buf(0, 0, CUtlBuffer::TEXT_BUFFER);
	
	buf.Printf("\"items_game\"\n{\n");
	buf.Printf("\t\"items\"\n\t{\n");
	
	// Merge workshop item schemas
	FOR_EACH_MAP_FAST(m_mapCustomItems, i)
	{
		CFCustomItemEntry_t* pEntry = m_mapCustomItems[i];
		if (pEntry->m_pKeyValues)
		{
			// Find the "items" section in this schema
			KeyValues* pSourceItems = pEntry->m_pKeyValues->FindKey("items");
			if (pSourceItems)
			{
				// Copy each item definition
				for (KeyValues* pItemDef = pSourceItems->GetFirstSubKey(); pItemDef; pItemDef = pItemDef->GetNextKey())
				{
					// Make a copy and ensure it has moditem flag
					KeyValues* pClone = pItemDef->MakeCopy();
					
					// Add moditem flag if not present
					if (pClone->GetInt("moditem", 0) == 0)
					{
						pClone->SetInt("moditem", 1);
					}
					
					// Write this item definition to the buffer
					CUtlBuffer itemBuf(0, 0, CUtlBuffer::TEXT_BUFFER);
					pClone->RecursiveSaveToFile(itemBuf, 2); // indent level 2 (under items_game/items)
					buf.Put(itemBuf.Base(), itemBuf.TellPut());
					pClone->deleteThis();
					
					nMerged++;
					CFWorkshopMsg("  Merged item def '%s' from workshop item %llu\n", pItemDef->GetName(), pEntry->m_nWorkshopID);
				}
			}
		}
	}
	
	// Merge loose file schemas
	FOR_EACH_VEC(m_vecLooseSchemas, i)
	{
		CFCustomItemEntry_t* pEntry = m_vecLooseSchemas[i];
		if (pEntry->m_pKeyValues)
		{
			// Find the "items" section in this schema
			KeyValues* pSourceItems = pEntry->m_pKeyValues->FindKey("items");
			if (pSourceItems)
			{
				// Copy each item definition
				for (KeyValues* pItemDef = pSourceItems->GetFirstSubKey(); pItemDef; pItemDef = pItemDef->GetNextKey())
				{
					// Make a copy and ensure it has moditem flag
					KeyValues* pClone = pItemDef->MakeCopy();
					
					// Add moditem flag if not present
					if (pClone->GetInt("moditem", 0) == 0)
					{
						pClone->SetInt("moditem", 1);
					}
					
					// Write this item definition to the buffer
					CUtlBuffer itemBuf(0, 0, CUtlBuffer::TEXT_BUFFER);
					pClone->RecursiveSaveToFile(itemBuf, 2); // indent level 2 (under items_game/items)
					buf.Put(itemBuf.Base(), itemBuf.TellPut());
					pClone->deleteThis();
					
					nMerged++;
					CFWorkshopMsg("  Merged item def '%s' from loose file %s\n", pItemDef->GetName(), pEntry->m_strSchemaFile.Get());
				}
			}
		}
	}
	
	buf.Printf("\t}\n");
	buf.Printf("}\n");
	
	// Always write the file (even if empty) to clear old items
	if (g_pFullFileSystem->WriteFile(szPath, "GAME", buf))
	{
		CFWorkshopMsg("Successfully wrote %d item definitions to %s\n", nMerged, szPath);
	}
	else
	{
		CFWorkshopMsg("Failed to write merged schema to %s\n", szPath);
	}
}

//-----------------------------------------------------------------------------
// Workshop integration
//-----------------------------------------------------------------------------
bool CCFCustomItemSchemaManager::RegisterWorkshopItem(CCFWorkshopItem* pItem)
{
	if (!pItem)
	{
		CFWorkshopMsg("RegisterWorkshopItem: pItem is NULL\n");
		return false;
	}
	
	PublishedFileId_t workshopID = pItem->GetFileID();
	CFWorkshopMsg("RegisterWorkshopItem: Processing workshop item %llu\n", workshopID);
	
	// Check if already registered
	if (IsWorkshopItemLoaded(workshopID))
	{
		CFWorkshopMsg("Workshop item %llu already registered\n", workshopID);
		return true;
	}
	
	// Get install path - this will tell us if the item is actually available
	char szInstallPath[MAX_PATH];
	if (!pItem->GetInstallPath(szInstallPath, sizeof(szInstallPath)))
	{
		CFWorkshopMsg("RegisterWorkshopItem: Item %llu has no install path (not ready)\n", workshopID);
		return false;
	}
	
	CFWorkshopMsg("Workshop item %llu install path: %s\n", workshopID, szInstallPath);
	
	// Get current map name for map-specific schemas
	const char* pszMapName = m_strCurrentMap.Get();
	
	// Look for custom_items_game.txt in multiple possible locations
	char szSchemaPath[MAX_PATH];
	char szMapSchemaPath[MAX_PATH];
	
	// Priority 1: Map-specific schema (e.g., ctf_2fort_items_game.txt)
	if (pszMapName && pszMapName[0])
	{
		// Try: scripts/items/[mapname]_items_game.txt
		V_snprintf(szMapSchemaPath, sizeof(szMapSchemaPath), "%s/scripts/items/%s_items_game.txt", szInstallPath, pszMapName);
		CFWorkshopMsg("Checking for map-specific schema at: %s\n", szMapSchemaPath);
		
		if (filesystem->FileExists(szMapSchemaPath, "GAME"))
		{
			CFWorkshopMsg("Found map-specific custom item schema for workshop item %llu at: %s\n", workshopID, szMapSchemaPath);
			return LoadCustomItemSchema(szMapSchemaPath, workshopID, pszMapName);
		}
		
		// Try: scripts/[mapname]_items_game.txt
		V_snprintf(szMapSchemaPath, sizeof(szMapSchemaPath), "%s/scripts/%s_items_game.txt", szInstallPath, pszMapName);
		CFWorkshopMsg("Checking for map-specific schema at: %s\n", szMapSchemaPath);
		
		if (filesystem->FileExists(szMapSchemaPath, "GAME"))
		{
			CFWorkshopMsg("Found map-specific custom item schema for workshop item %llu at: %s\n", workshopID, szMapSchemaPath);
			return LoadCustomItemSchema(szMapSchemaPath, workshopID, pszMapName);
		}
		
		// Try: [mapname]_items_game.txt (root)
		V_snprintf(szMapSchemaPath, sizeof(szMapSchemaPath), "%s/%s_items_game.txt", szInstallPath, pszMapName);
		CFWorkshopMsg("Checking for map-specific schema at: %s\n", szMapSchemaPath);
		
		if (filesystem->FileExists(szMapSchemaPath, "GAME"))
		{
			CFWorkshopMsg("Found map-specific custom item schema for workshop item %llu at: %s\n", workshopID, szMapSchemaPath);
			return LoadCustomItemSchema(szMapSchemaPath, workshopID, pszMapName);
		}
	}
	
	// Priority 2: Global schema (custom_items_game.txt)
	// Try: scripts/items/custom_items_game.txt
	V_snprintf(szSchemaPath, sizeof(szSchemaPath), "%s/scripts/items/custom_items_game.txt", szInstallPath);
	CFWorkshopMsg("Checking for schema at: %s\n", szSchemaPath);
	
	if (filesystem->FileExists(szSchemaPath, "GAME"))
	{
		CFWorkshopMsg("Found custom item schema for workshop item %llu at: %s\n", workshopID, szSchemaPath);
		return LoadCustomItemSchema(szSchemaPath, workshopID, NULL);
	}
	
	// Try: scripts/custom_items_game.txt
	V_snprintf(szSchemaPath, sizeof(szSchemaPath), "%s/scripts/custom_items_game.txt", szInstallPath);
	CFWorkshopMsg("Checking for schema at: %s\n", szSchemaPath);
	
	if (filesystem->FileExists(szSchemaPath, "GAME"))
	{
		CFWorkshopMsg("Found custom item schema for workshop item %llu at: %s\n", workshopID, szSchemaPath);
		return LoadCustomItemSchema(szSchemaPath, workshopID, NULL);
	}
	
	// Try: custom_items_game.txt (root)
	V_snprintf(szSchemaPath, sizeof(szSchemaPath), "%s/custom_items_game.txt", szInstallPath);
	CFWorkshopMsg("Checking for schema at: %s\n", szSchemaPath);
	
	if (filesystem->FileExists(szSchemaPath, "GAME"))
	{
		CFWorkshopMsg("Found custom item schema for workshop item %llu at: %s\n", workshopID, szSchemaPath);
		return LoadCustomItemSchema(szSchemaPath, workshopID, NULL);
	}
	
	// No custom schema file - that's okay, not all workshop items need one
	CFWorkshopMsg("Workshop item %llu has no custom item schema\n", workshopID);
	return false;
}

void CCFCustomItemSchemaManager::UnregisterWorkshopItem(PublishedFileId_t workshopID)
{
	int idx = m_mapCustomItems.Find(workshopID);
	if (idx == m_mapCustomItems.InvalidIndex())
		return;
	
	CFWorkshopMsg("Unregistering workshop item %llu from custom schema\n", workshopID);
	
	CFCustomItemEntry_t* pEntry = m_mapCustomItems[idx];
	
	// Remove from inventory
	RemoveInventoryItemsForWorkshopID(workshopID);
	
	// Remove from schema
	if (pEntry->m_nDefIndex != INVALID_ITEM_DEF_INDEX)
	{
		RemoveCustomDefinitionFromSchema(pEntry->m_nDefIndex);
		ReleaseDefIndex(pEntry->m_nDefIndex);
	}
	
	m_mapCustomItems.RemoveAt(idx);
	delete pEntry;
	
	CFWorkshopMsg("Unregistered workshop item %llu\n", workshopID);
}

bool CCFCustomItemSchemaManager::IsWorkshopItemLoaded(PublishedFileId_t workshopID) const
{
	return m_mapCustomItems.Find(workshopID) != m_mapCustomItems.InvalidIndex();
}

//-----------------------------------------------------------------------------
// Item management
//-----------------------------------------------------------------------------
CFCustomItemEntry_t* CCFCustomItemSchemaManager::GetCustomItemEntry(PublishedFileId_t workshopID)
{
	int idx = m_mapCustomItems.Find(workshopID);
	if (idx == m_mapCustomItems.InvalidIndex())
		return NULL;
	
	return m_mapCustomItems[idx];
}

const CFCustomItemEntry_t* CCFCustomItemSchemaManager::GetCustomItemEntry(PublishedFileId_t workshopID) const
{
	int idx = m_mapCustomItems.Find(workshopID);
	if (idx == m_mapCustomItems.InvalidIndex())
		return NULL;
	
	return m_mapCustomItems[idx];
}

//-----------------------------------------------------------------------------
// Definition index allocation
//-----------------------------------------------------------------------------
item_definition_index_t CCFCustomItemSchemaManager::AllocateDefIndex()
{
	// Find next available index
	while (m_nNextDefIndex <= CF_CUSTOM_ITEM_DEF_END)
	{
		if (m_mapDefToWorkshop.Find(m_nNextDefIndex) == m_mapDefToWorkshop.InvalidIndex())
		{
			return m_nNextDefIndex++;
		}
		m_nNextDefIndex++;
	}
	
	// Out of custom def indices!
	CFWorkshopWarning("Out of custom item definition indices!\n");
	return INVALID_ITEM_DEF_INDEX;
}

void CCFCustomItemSchemaManager::ReleaseDefIndex(item_definition_index_t defIndex)
{
	m_mapDefToWorkshop.Remove(defIndex);
}

bool CCFCustomItemSchemaManager::IsCustomDefIndex(item_definition_index_t defIndex) const
{
	return (defIndex >= CF_CUSTOM_ITEM_DEF_START && defIndex <= CF_CUSTOM_ITEM_DEF_END);
}

//-----------------------------------------------------------------------------
// Parse custom item definition
//-----------------------------------------------------------------------------
bool CCFCustomItemSchemaManager::ParseCustomItemDefinition(KeyValues* pKV, PublishedFileId_t workshopID, item_definition_index_t defIndex)
{
	if (!pKV)
		return false;
	
	// For now, we use a simplified approach:
	// Custom items are treated like mod items and accessed via def index
	// The game's existing mod item system handles them through AddModItem()
	
	// Store the mapping
	m_mapDefToWorkshop.Insert(defIndex, workshopID);
	
	CFWorkshopMsg("Registered custom item definition %d for workshop item %llu\n", defIndex, workshopID);
	
	// The actual item definition should exist in the schema already if items_game.txt
	// was loaded correctly. Check if it exists:
	CEconItemDefinition* pItemDef = ItemSystem()->GetItemSchema()->GetItemDefinition(defIndex);
	if (pItemDef)
	{
		CFWorkshopMsg("Item definition %d found in schema: %s\n", defIndex, pItemDef->GetDefinitionName());
		return true;
	}
	else
	{
		CFWorkshopWarning("Item definition %d not found in schema after loading\n", defIndex);
		// This is expected - the schema needs to be reloaded with our custom file merged in
		// For now, the mod item system will handle creating items with this def index
		return true;
	}
}

bool CCFCustomItemSchemaManager::IntegrateCustomDefinitionIntoSchema(CEconItemDefinition* pDef)
{
	// This function is no longer needed with the mod item approach
	// The mod item system handles custom items automatically
	return true;
}

void CCFCustomItemSchemaManager::RemoveCustomDefinitionFromSchema(item_definition_index_t defIndex)
{
	// TODO: Remove the definition from the main item schema
	// This requires access to the schema's internal item map
}

//-----------------------------------------------------------------------------
// Inventory integration
//-----------------------------------------------------------------------------
void CCFCustomItemSchemaManager::CreateInventoryItemsForCustomSchema()
{
#ifdef CLIENT_DLL
	// Only on client
	CFWorkshopMsg("Creating inventory items for custom schemas...\n");
	
	// Process workshop schemas
	FOR_EACH_MAP_FAST(m_mapCustomItems, i)
	{
		CFCustomItemEntry_t* pEntry = m_mapCustomItems[i];
		CFWorkshopMsg("Checking entry: Workshop ID %llu, DefIndex %d, Loaded=%d, Enabled=%d\n",
			pEntry->m_nWorkshopID, pEntry->m_nDefIndex, pEntry->m_bLoaded, pEntry->m_bEnabled);
		
		if (pEntry->m_bLoaded && pEntry->m_bEnabled && pEntry->m_nDefIndex != INVALID_ITEM_DEF_INDEX)
		{
			// Check if this item already exists
			int count = TFInventoryManager()->GetModItemCount();
			bool bAlreadyExists = false;
			for (int j = 0; j < count; j++)
			{
				CEconItemView* pExisting = TFInventoryManager()->GetModItem(j);
				if (pExisting && pExisting->GetItemDefIndex() == pEntry->m_nDefIndex)
				{
					bAlreadyExists = true;
					CFWorkshopMsg("Item %d already exists in mod inventory\n", pEntry->m_nDefIndex);
					break;
				}
			}
			
			if (!bAlreadyExists)
			{
				// Use the existing AddModItem system
				CEconItemView* pItemView = TFInventoryManager()->AddModItem(pEntry->m_nDefIndex);
				if (pItemView)
				{
					CFWorkshopMsg("Created inventory item for custom item %d (workshop ID %llu)\n", 
						pEntry->m_nDefIndex, pEntry->m_nWorkshopID);
					
					// Verify it was added
					CEconItemDefinition* pDef = ItemSystem()->GetItemSchema()->GetItemDefinition(pEntry->m_nDefIndex);
					if (pDef)
					{
						CFWorkshopMsg("  Item definition name: %s\n", pDef->GetDefinitionName());
					}
					else
					{
						CFWorkshopWarning("  Item definition %d not found in schema!\n", pEntry->m_nDefIndex);
					}
				}
				else
				{
					CFWorkshopWarning("Failed to create inventory item for custom item %d\n", pEntry->m_nDefIndex);
				}
			}
		}
	}
	
	// Process loose file schemas
	FOR_EACH_VEC(m_vecLooseSchemas, i)
	{
		CFCustomItemEntry_t* pEntry = m_vecLooseSchemas[i];
		CFWorkshopMsg("Checking loose entry: DefIndex %d, Loaded=%d, Enabled=%d\n",
			pEntry->m_nDefIndex, pEntry->m_bLoaded, pEntry->m_bEnabled);
		
		if (pEntry->m_bLoaded && pEntry->m_bEnabled && pEntry->m_nDefIndex != INVALID_ITEM_DEF_INDEX)
		{
			// Check if this item already exists
			int count = TFInventoryManager()->GetModItemCount();
			bool bAlreadyExists = false;
			for (int j = 0; j < count; j++)
			{
				CEconItemView* pExisting = TFInventoryManager()->GetModItem(j);
				if (pExisting && pExisting->GetItemDefIndex() == pEntry->m_nDefIndex)
				{
					bAlreadyExists = true;
					CFWorkshopMsg("Item %d already exists in mod inventory\n", pEntry->m_nDefIndex);
					break;
				}
			}
			
			if (!bAlreadyExists)
			{
				// Use the existing AddModItem system
				CEconItemView* pItemView = TFInventoryManager()->AddModItem(pEntry->m_nDefIndex);
				if (pItemView)
				{
					CFWorkshopMsg("Created inventory item for loose schema item %d\n", pEntry->m_nDefIndex);
					
					// Verify it was added
					CEconItemDefinition* pDef = ItemSystem()->GetItemSchema()->GetItemDefinition(pEntry->m_nDefIndex);
					if (pDef)
					{
						CFWorkshopMsg("  Item definition name: %s\n", pDef->GetDefinitionName());
					}
					else
					{
						CFWorkshopWarning("  Item definition %d not found in schema!\n", pEntry->m_nDefIndex);
					}
				}
				else
				{
					CFWorkshopWarning("Failed to create inventory item for loose schema item %d\n", pEntry->m_nDefIndex);
				}
			}
		}
	}
#endif
}

void CCFCustomItemSchemaManager::RemoveInventoryItemsForWorkshopID(PublishedFileId_t workshopID)
{
#ifdef CLIENT_DLL
	// Find the custom item entry
	int idx = m_mapCustomItems.Find(workshopID);
	if (idx == m_mapCustomItems.InvalidIndex())
		return;
	
	CFCustomItemEntry_t* pEntry = m_mapCustomItems[idx];
	if (pEntry->m_nDefIndex == INVALID_ITEM_DEF_INDEX)
		return;
	
	// Remove from mod loadout items
	int count = TFInventoryManager()->GetModItemCount();
	for (int i = count - 1; i >= 0; i--)
	{
		CEconItemView* pItem = TFInventoryManager()->GetModItem(i);
		if (pItem && pItem->GetItemDefIndex() == pEntry->m_nDefIndex)
		{
			// TODO: Need to add a RemoveModItem function to TFInventoryManager
			CFWorkshopMsg("Would remove inventory item for custom item %d\n", pEntry->m_nDefIndex);
		}
	}
#endif
}

//-----------------------------------------------------------------------------
// Console command support
//-----------------------------------------------------------------------------
void CCFCustomItemSchemaManager::DumpCustomItems()
{
	Msg("=== Custom Item Schema ===\n");
	Msg("Current Map: %s\n", m_strCurrentMap.Get());
	Msg("Loaded %d workshop schemas, %d loose schemas\n\n", m_mapCustomItems.Count(), m_vecLooseSchemas.Count());
	
	Msg("Workshop Schemas:\n");
	FOR_EACH_MAP_FAST(m_mapCustomItems, i)
	{
		CFCustomItemEntry_t* pEntry = m_mapCustomItems[i];
		Msg("  Workshop ID: %llu\n", pEntry->m_nWorkshopID);
		Msg("    Def Index: %d\n", pEntry->m_nDefIndex);
		Msg("    Schema File: %s\n", pEntry->m_strSchemaFile.Get());
		Msg("    Map Filter: %s\n", pEntry->m_strMapName.Get()[0] ? pEntry->m_strMapName.Get() : "(all maps)");
		Msg("    Loaded: %s\n", pEntry->m_bLoaded ? "Yes" : "No");
		Msg("    Enabled: %s\n\n", pEntry->m_bEnabled ? "Yes" : "No");
	}
	
	Msg("\nLoose File Schemas:\n");
	FOR_EACH_VEC(m_vecLooseSchemas, i)
	{
		CFCustomItemEntry_t* pEntry = m_vecLooseSchemas[i];
		Msg("  Schema File: %s\n", pEntry->m_strSchemaFile.Get());
		Msg("    Def Index: %d\n", pEntry->m_nDefIndex);
		Msg("    Map Filter: %s\n", pEntry->m_strMapName.Get()[0] ? pEntry->m_strMapName.Get() : "(all maps)");
		Msg("    Loaded: %s\n\n", pEntry->m_bLoaded ? "Yes" : "No");
	}
}

//-----------------------------------------------------------------------------// Map-specific schema loading
//-----------------------------------------------------------------------------
void CCFCustomItemSchemaManager::LoadSchemasForCurrentMap()
{
	CFWorkshopMsg("Loading schemas for map: %s\n", m_strCurrentMap.Get());
	
	// Clear all previous schemas
	ClearAllCustomSchemas();
	
	// Scan loose files in maps folder
	ScanMapsFolder();
	
	// Load workshop item schemas (if appropriate for server type)
	uint32 nSubscribed = CFWorkshop()->GetSubscribedItemCount();
	int nLoaded = 0;
	
	for (uint32 i = 0; i < nSubscribed; i++)
	{
		PublishedFileId_t fileID = CFWorkshop()->GetSubscribedItem(i);
		
		// Check if this schema should load based on server type
		if (!ShouldLoadSchema(fileID))
		{
			CFWorkshopMsg("Skipping workshop item %llu (not authorized for this server type)\n", fileID);
			continue;
		}
		
		CCFWorkshopItem* pItem = CFWorkshop()->GetItem(fileID);
		if (pItem && RegisterWorkshopItem(pItem))
		{
			nLoaded++;
		}
	}
	
	CFWorkshopMsg("Loaded %d workshop schemas for map %s\n", nLoaded, m_strCurrentMap.Get());
	
	// Write merged schema file
	WriteMergedCustomSchema();
	
#ifdef CLIENT_DLL
	// Reload the schema to pick up the new items
	ReloadClientItemSchema();
	CFWorkshopMsg("Schema reloaded for map %s\n", m_strCurrentMap.Get());
#else
	// Server: Schema will be automatically reloaded when items are requested
	CFWorkshopMsg("Server schema updated for map %s\n", m_strCurrentMap.Get());
#endif
}

void CCFCustomItemSchemaManager::ScanMapsFolder()
{
	CFWorkshopMsg("Scanning maps folder for loose schema files...\n");
	
	const char* pszMapName = m_strCurrentMap.Get();
	if (!pszMapName || !pszMapName[0])
	{
		CFWorkshopMsg("No map loaded yet, skipping loose file scan\n");
		return;
	}
	
	CFWorkshopMsg("Current map name: '%s'\n", pszMapName);
	
	// Look for custom_items_game.txt (global)
	char szGlobalSchema[MAX_PATH];
	V_snprintf(szGlobalSchema, sizeof(szGlobalSchema), "maps/custom_items_game.txt");
	
	CFWorkshopMsg("Checking for global schema at: %s\n", szGlobalSchema);
	bool bGlobalExists = filesystem->FileExists(szGlobalSchema, "GAME");
	CFWorkshopMsg("File exists: %s\n", bGlobalExists ? "YES" : "NO");
	
	if (bGlobalExists)
	{
		CFWorkshopMsg("Found global loose schema: %s\n", szGlobalSchema);
		bool bLoaded = LoadCustomItemSchema(szGlobalSchema, 0, NULL); // workshopID=0 for loose files, pass relative path
		CFWorkshopMsg("Load result: %s\n", bLoaded ? "SUCCESS" : "FAILED");
	}
	
	// Look for map-specific schema (e.g., ctf_2fort_items_game.txt)
	char szMapSchema[MAX_PATH];
	V_snprintf(szMapSchema, sizeof(szMapSchema), "maps/%s_items_game.txt", pszMapName);
	
	CFWorkshopMsg("Checking for map-specific schema at: %s\n", szMapSchema);
	bool bMapExists = filesystem->FileExists(szMapSchema, "GAME");
	CFWorkshopMsg("File exists: %s\n", bMapExists ? "YES" : "NO");
	
	if (bMapExists)
	{
		CFWorkshopMsg("Found map-specific loose schema: %s\n", szMapSchema);
		bool bLoaded = LoadCustomItemSchema(szMapSchema, 0, pszMapName);
		CFWorkshopMsg("Load result: %s\n", bLoaded ? "SUCCESS" : "FAILED");
	}
	
	CFWorkshopMsg("Finished scanning maps folder, loaded %d loose schemas\n", m_vecLooseSchemas.Count());
}

//-----------------------------------------------------------------------------
// Server type detection
//-----------------------------------------------------------------------------
bool CCFCustomItemSchemaManager::IsListenServer() const
{
#ifdef CLIENT_DLL
	// On client, check if we're connected to a listen server
	if (engine->IsInGame())
	{
		// We're in a game - assume listen server if not on dedicated
		// (client can't directly check if server is dedicated)
		return true;
	}
#else
	// On server, check if this is a listen server (not dedicated)
	if (!engine->IsDedicatedServer())
	{
		return true;
	}
#endif
	return false;
}

bool CCFCustomItemSchemaManager::IsListenServerHost() const
{
#ifdef CLIENT_DLL
	C_TFPlayer* pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (!pLocalPlayer)
		return false;
	
	// Check if we're the host (listen server)
	// On client, we can't check IsDedicatedServer, but we can check if we're player 1
	if (engine->IsInGame())
	{
		// On a listen server, the host is player index 1
		return pLocalPlayer->entindex() == 1;
	}
#endif
	return false;
}

bool CCFCustomItemSchemaManager::ShouldLoadSchema(PublishedFileId_t workshopID) const
{
#ifdef CLIENT_DLL
	// On client side
	// On LAN/offline, load all schemas
	if (!engine->IsInGame())
	{
		return true;
	}
	
	// On listen servers, only load the host's schemas
	if (IsListenServer())
	{
		if (IsListenServerHost())
		{
			// We're the host, load all our subscribed items
			return true;
		}
		else
		{
			// We're a client on a listen server, don't load our schemas
			// The host's schemas take priority
			CFWorkshopMsg("Client on listen server - schema %llu disabled\n", workshopID);
			return false;
		}
	}
#else
	// On server side - always load all schemas
	// (both dedicated and listen servers)
	return true;
#endif
	
	// Default: load the schema
	return true;
}

//-----------------------------------------------------------------------------
// Map utilities
//-----------------------------------------------------------------------------
const char* CCFCustomItemSchemaManager::GetCurrentMapName() const
{
	return m_strCurrentMap.Get();
}

bool CCFCustomItemSchemaManager::IsMapSpecificSchema(const char* pszFileName, const char* pszMapName) const
{
	if (!pszFileName || !pszMapName)
		return false;
	
	// Extract filename from path
	const char* pszName = V_GetFileName(pszFileName);
	
	// Check if it matches the pattern: [mapname]_items_game.txt
	char szExpected[MAX_PATH];
	V_snprintf(szExpected, sizeof(szExpected), "%s_items_game.txt", pszMapName);
	
	return V_stricmp(pszName, szExpected) == 0;
}

//-----------------------------------------------------------------------------// Console commands
//-----------------------------------------------------------------------------
CON_COMMAND(cf_dump_custom_items, "Dump all loaded custom items")
{
	CFCustomItemSchema()->DumpCustomItems();
}

CON_COMMAND(cf_debug_item, "Debug an item definition by index")
{
	if (args.ArgC() < 2)
	{
		Msg("Usage: cf_debug_item <def_index>\n");
		return;
	}
	
	int defIndex = atoi(args[1]);
	CEconItemDefinition* pDef = ItemSystem()->GetItemSchema()->GetItemDefinition(defIndex);
	
	if (!pDef)
	{
		Msg("Item definition %d not found in schema!\n", defIndex);
		return;
	}
	
	Msg("=== Item Definition %d ===\n", defIndex);
	Msg("Name: %s\n", pDef->GetDefinitionName());
	Msg("Item Name: %s\n", pDef->GetItemBaseName());
	Msg("Item Class: %s\n", pDef->GetItemClass());
	Msg("Quality: %d\n", pDef->GetQuality());
	Msg("Enabled: %d\n", pDef->BEnabled());
	
	// Check if it's in the mod items map
	const CUtlMap<int, CEconItemDefinition*, int>& mapModItems = ItemSystem()->GetItemSchema()->GetSoloItemDefinitionMap();
	bool bInModItems = mapModItems.Find(defIndex) != mapModItems.InvalidIndex();
	Msg("In Mod Items Map: %d\n", bInModItems);
	
	// Try to get raw definition keys
	KeyValues* pKV = pDef->GetRawDefinition();
	if (pKV)
	{
		Msg("Has raw definition: Yes\n");
		Msg("Item Slot: %s\n", pKV->GetString("item_slot", "NOT SET"));
		Msg("Moditem flag: %d\n", pKV->GetInt("moditem", 0));
	}
	else
	{
		Msg("Has raw definition: No\n");
	}
}

CON_COMMAND(cf_force_reload_schemas, "Force reload all schemas")
{
	Msg("Force reloading schemas...\n");
	CFCustomItemSchema()->LoadSchemasForCurrentMap();
}

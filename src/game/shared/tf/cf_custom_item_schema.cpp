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
#else
#include "tf_player.h"
#endif

#include "econ_item_inventory.h"

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
	// Refresh custom items for level precaching
	CreateInventoryItemsForCustomSchema();
}

//-----------------------------------------------------------------------------
// Schema management
//-----------------------------------------------------------------------------
bool CCFCustomItemSchemaManager::LoadCustomItemSchema(const char* pszSchemaFile, PublishedFileId_t workshopID)
{
	if (!pszSchemaFile || !pszSchemaFile[0])
	{
		CFWorkshopWarning("LoadCustomItemSchema: Invalid schema file path\n");
		return false;
	}
	
	if (workshopID == 0)
	{
		CFWorkshopWarning("LoadCustomItemSchema: Invalid workshop ID\n");
		return false;
	}
	
	// Check if already loaded
	int idx = m_mapCustomItems.Find(workshopID);
	if (idx != m_mapCustomItems.InvalidIndex())
	{
		CFWorkshopMsg("Custom schema for workshop item %llu already loaded\n", workshopID);
		return true;
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
		m_mapCustomItems.Insert(workshopID, pEntry);
		CFWorkshopMsg("Successfully loaded custom schema for workshop item %llu\n", workshopID);
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
	
	// Clear all schemas
	ClearAllCustomSchemas();
	
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
	
	CFWorkshopMsg("Reloaded %d/%d custom item schemas\n", nSuccess, vecAllWorkshopIDs.Count());
	
	// Write merged schema file
	if (nSuccess > 0)
	{
		WriteMergedCustomSchema();
	}
	
	// Refresh inventory
	CreateInventoryItemsForCustomSchema();
	
	return nSuccess > 0;
}

void CCFCustomItemSchemaManager::ClearAllCustomSchemas()
{
	CFWorkshopMsg("Clearing all custom item schemas...\n");
	
	// Remove all definitions from schema
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
	
	m_mapCustomItems.Purge();
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
	
	buf.Printf("\t}\n");
	buf.Printf("}\n");
	
	if (nMerged > 0)
	{
		// Write to file
		if (g_pFullFileSystem->WriteFile(szPath, "GAME", buf))
		{
			CFWorkshopMsg("Successfully wrote %d item definitions to %s\n", nMerged, szPath);
		}
		else
		{
			CFWorkshopMsg("Failed to write merged schema to %s\n", szPath);
		}
	}
	else
	{
		CFWorkshopMsg("No item definitions to merge\n");
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
	
	// Look for items_game.txt in multiple possible locations
	char szSchemaPath[MAX_PATH];
	
	// Try: scripts/items/items_game.txt
	V_snprintf(szSchemaPath, sizeof(szSchemaPath), "%s/scripts/items/items_game.txt", szInstallPath);
	CFWorkshopMsg("Checking for schema at: %s\n", szSchemaPath);
	
	if (filesystem->FileExists(szSchemaPath, "GAME"))
	{
		CFWorkshopMsg("Found custom item schema for workshop item %llu at: %s\n", workshopID, szSchemaPath);
		return LoadCustomItemSchema(szSchemaPath, workshopID);
	}
	
	// Try: scripts/items_game.txt
	V_snprintf(szSchemaPath, sizeof(szSchemaPath), "%s/scripts/items_game.txt", szInstallPath);
	CFWorkshopMsg("Checking for schema at: %s\n", szSchemaPath);
	
	if (filesystem->FileExists(szSchemaPath, "GAME"))
	{
		CFWorkshopMsg("Found custom item schema for workshop item %llu at: %s\n", workshopID, szSchemaPath);
		return LoadCustomItemSchema(szSchemaPath, workshopID);
	}
	
	// Try: items_game.txt (root)
	V_snprintf(szSchemaPath, sizeof(szSchemaPath), "%s/items_game.txt", szInstallPath);
	CFWorkshopMsg("Checking for schema at: %s\n", szSchemaPath);
	
	if (filesystem->FileExists(szSchemaPath, "GAME"))
	{
		CFWorkshopMsg("Found custom item schema for workshop item %llu at: %s\n", workshopID, szSchemaPath);
		return LoadCustomItemSchema(szSchemaPath, workshopID);
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
	
	// For each loaded custom item, create an inventory item
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
	Msg("Loaded %d custom item schemas\n\n", m_mapCustomItems.Count());
	
	FOR_EACH_MAP_FAST(m_mapCustomItems, i)
	{
		CFCustomItemEntry_t* pEntry = m_mapCustomItems[i];
		Msg("Workshop ID: %llu\n", pEntry->m_nWorkshopID);
		Msg("  Def Index: %d\n", pEntry->m_nDefIndex);
		Msg("  Schema File: %s\n", pEntry->m_strSchemaFile.Get());
		Msg("  Loaded: %s\n", pEntry->m_bLoaded ? "Yes" : "No");
		Msg("  Enabled: %s\n\n", pEntry->m_bEnabled ? "Yes" : "No");
	}
}

//-----------------------------------------------------------------------------
// Console commands
//-----------------------------------------------------------------------------
CON_COMMAND(cf_dump_custom_items, "Dump all loaded custom items")
{
	CFCustomItemSchema()->DumpCustomItems();
}

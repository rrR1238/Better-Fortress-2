//====== Copyright Custom Fortress 2, All rights reserved. =================
//
// Custom Item Schema Manager
// Handles loading and integration of custom item definitions from Workshop
//
//=============================================================================

#ifndef CF_CUSTOM_ITEM_SCHEMA_H
#define CF_CUSTOM_ITEM_SCHEMA_H
#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "econ_item_schema.h"
#include "econ_item_view.h"
#include "utlvector.h"
#include "utlmap.h"
#include "utlstring.h"

// Custom item def index range - use high values to avoid conflicts
#define CF_CUSTOM_ITEM_DEF_START 50000
#define CF_CUSTOM_ITEM_DEF_END   65535

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class CCFWorkshopItem;
class CEconItemDefinition;

//-----------------------------------------------------------------------------
// Custom item entry - represents a workshop item or loose schema that has been loaded
//-----------------------------------------------------------------------------
struct CFCustomItemEntry_t
{
	PublishedFileId_t m_nWorkshopID;			// Steam Workshop file ID (0 for loose files)
	item_definition_index_t m_nDefIndex;		// Assigned definition index
	CUtlString m_strSchemaFile;					// Path to custom_items_game.txt
	CUtlString m_strMapName;					// Map name filter (empty = all maps)
	bool m_bLoaded;								// Successfully loaded into schema
	bool m_bEnabled;							// Currently enabled in inventory
	bool m_bFromLooseFile;						// Loaded from maps folder (not workshop)
	KeyValues* m_pKeyValues;					// Loaded KeyValues data
	
	CFCustomItemEntry_t()
		: m_nWorkshopID(0)
		, m_nDefIndex(INVALID_ITEM_DEF_INDEX)
		, m_bLoaded(false)
		, m_bEnabled(false)
		, m_bFromLooseFile(false)
		, m_pKeyValues(NULL)
	{
	}
	
	~CFCustomItemEntry_t()
	{
		if (m_pKeyValues)
		{
			m_pKeyValues->deleteThis();
			m_pKeyValues = NULL;
		}
	}
};

//-----------------------------------------------------------------------------
// Custom Item Schema Manager
// Loads custom item definitions from workshop items and integrates them
// into the main item schema
//-----------------------------------------------------------------------------
class CCFCustomItemSchemaManager : public CAutoGameSystem
{
public:
	CCFCustomItemSchemaManager();
	
	// CAutoGameSystem
	virtual bool Init() OVERRIDE;
	virtual void PostInit() OVERRIDE;
	virtual void Shutdown() OVERRIDE;
	virtual void LevelInitPreEntity() OVERRIDE;
	virtual void LevelShutdownPreEntity() OVERRIDE;
	
	// Schema management
	bool LoadCustomItemSchema(const char* pszSchemaFile, PublishedFileId_t workshopID, const char* pszMapFilter = NULL);
	bool ReloadAllCustomSchemas();
	void ClearAllCustomSchemas();
	void WriteMergedCustomSchema();
	void LoadSchemasForCurrentMap(); // Load map-specific and global schemas
	
	// Loose file scanning
	void ScanMapsFolder(); // Scan maps folder for loose schema files
	
	// Workshop integration
	bool RegisterWorkshopItem(CCFWorkshopItem* pItem);
	void UnregisterWorkshopItem(PublishedFileId_t workshopID);
	bool IsWorkshopItemLoaded(PublishedFileId_t workshopID) const;
	
	// Item management
	int GetCustomItemCount() const { return m_mapCustomItems.Count(); }
	CFCustomItemEntry_t* GetCustomItemEntry(PublishedFileId_t workshopID);
	const CFCustomItemEntry_t* GetCustomItemEntry(PublishedFileId_t workshopID) const;
	
	// Definition index allocation
	item_definition_index_t AllocateDefIndex();
	void ReleaseDefIndex(item_definition_index_t defIndex);
	bool IsCustomDefIndex(item_definition_index_t defIndex) const;
	
	// Inventory integration
	void CreateInventoryItemsForCustomSchema();
	void RemoveInventoryItemsForWorkshopID(PublishedFileId_t workshopID);
	
	// Console command support
	void DumpCustomItems();
	
	// Server type detection
	bool IsListenServer() const;
	bool IsListenServerHost() const;
	bool ShouldLoadSchema(PublishedFileId_t workshopID) const; // Check if schema should load based on server type
	
	// Map utilities
	const char* GetCurrentMapName() const;
	bool IsMapSpecificSchema(const char* pszFileName, const char* pszMapName) const;
	
private:
	bool ParseCustomItemDefinition(KeyValues* pKV, PublishedFileId_t workshopID, item_definition_index_t defIndex);
	bool IntegrateCustomDefinitionIntoSchema(CEconItemDefinition* pDef);
	void RemoveCustomDefinitionFromSchema(item_definition_index_t defIndex);
	
	CUtlMap<PublishedFileId_t, CFCustomItemEntry_t*> m_mapCustomItems;		// Workshop ID -> Custom item
	CUtlMap<item_definition_index_t, PublishedFileId_t> m_mapDefToWorkshop;	// Def index -> Workshop ID
	CUtlVector<CFCustomItemEntry_t*> m_vecLooseSchemas;					// Schemas loaded from loose files
	item_definition_index_t m_nNextDefIndex;								// Next available def index
	CUtlString m_strCurrentMap;												// Current map name
	bool m_bInitialized;
};

// Global accessor
CCFCustomItemSchemaManager* CFCustomItemSchema();

#endif // CF_CUSTOM_ITEM_SCHEMA_H

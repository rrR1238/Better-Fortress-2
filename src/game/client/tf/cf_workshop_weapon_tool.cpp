//====== Copyright Custom Fortress 2, All rights reserved. =================
//
// Workshop Weapon Mod Creation Tool Implementation
//
//=============================================================================

#include "cbase.h"
#include "cf_workshop_weapon_tool.h"
#include "cf_workshop_manager.h"
#include <vgui/IVGui.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/IInput.h>
#include <vgui_controls/FileOpenDialog.h>
#include <vgui_controls/DirectorySelectDialog.h>
#include <vgui_controls/MessageBox.h>
#include "vgui/ILocalize.h"
#include "ienginevgui.h"
#include "filesystem.h"
#include "tier1/KeyValues.h"
#include <direct.h>
#include <stdio.h>

#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Known weapon definitions - simplified list
//-----------------------------------------------------------------------------
namespace vgui
{

const KnownWeaponDef_t g_KnownWeapons[] = {
	// Soldier
	{ "tf_weapon_rocketlauncher", "Rocket Launcher", "models/weapons/c_models/c_rocketlauncher/c_rocketlauncher.mdl", 3, 0 },
	{ "tf_weapon_shotgun_soldier", "Shotgun (Soldier)", "models/weapons/c_models/c_shotgun/c_shotgun.mdl", 3, 1 },
	{ "tf_weapon_shovel", "Shovel", "models/weapons/c_models/c_shovel/c_shovel.mdl", 3, 2 },
	// Scout
	{ "tf_weapon_scattergun", "Scattergun", "models/weapons/c_models/c_scattergun/c_scattergun.mdl", 1, 0 },
	{ "tf_weapon_pistol_scout", "Pistol (Scout)", "models/weapons/c_models/c_pistol/c_pistol.mdl", 1, 1 },
	{ "tf_weapon_bat", "Bat", "models/weapons/c_models/c_bat/c_bat.mdl", 1, 2 },
	// Pyro
	{ "tf_weapon_flamethrower", "Flame Thrower", "models/weapons/c_models/c_flamethrower/c_flamethrower.mdl", 7, 0 },
	{ "tf_weapon_shotgun_pyro", "Shotgun (Pyro)", "models/weapons/c_models/c_shotgun/c_shotgun.mdl", 7, 1 },
	{ "tf_weapon_fireaxe", "Fire Axe", "models/weapons/c_models/c_fireaxe/c_fireaxe.mdl", 7, 2 },
	// Demoman
	{ "tf_weapon_grenadelauncher", "Grenade Launcher", "models/weapons/c_models/c_grenadelauncher/c_grenadelauncher.mdl", 4, 0 },
	{ "tf_weapon_pipebomblauncher", "Stickybomb Launcher", "models/weapons/c_models/c_stickybomb_launcher/c_stickybomb_launcher.mdl", 4, 1 },
	{ "tf_weapon_bottle", "Bottle", "models/weapons/c_models/c_bottle/c_bottle.mdl", 4, 2 },
	// Heavy
	{ "tf_weapon_minigun", "Minigun", "models/weapons/c_models/c_minigun/c_minigun.mdl", 6, 0 },
	{ "tf_weapon_shotgun_hwg", "Shotgun (Heavy)", "models/weapons/c_models/c_shotgun/c_shotgun.mdl", 6, 1 },
	{ "tf_weapon_fists", "Fists", "models/weapons/c_models/c_fists/c_fists.mdl", 6, 2 },
	// Engineer
	{ "tf_weapon_shotgun_primary", "Shotgun (Engineer)", "models/weapons/c_models/c_shotgun/c_shotgun.mdl", 9, 0 },
	{ "tf_weapon_pistol", "Pistol (Engineer)", "models/weapons/c_models/c_pistol/c_pistol.mdl", 9, 1 },
	{ "tf_weapon_wrench", "Wrench", "models/weapons/c_models/c_wrench/c_wrench.mdl", 9, 2 },
	// Medic
	{ "tf_weapon_syringegun_medic", "Syringe Gun", "models/weapons/c_models/c_syringegun/c_syringegun.mdl", 5, 0 },
	{ "tf_weapon_medigun", "Medi Gun", "models/weapons/c_models/c_medigun/c_medigun.mdl", 5, 1 },
	{ "tf_weapon_bonesaw", "Bonesaw", "models/weapons/c_models/c_bonesaw/c_bonesaw.mdl", 5, 2 },
	// Sniper
	{ "tf_weapon_sniperrifle", "Sniper Rifle", "models/weapons/c_models/c_sniperrifle/c_sniperrifle.mdl", 2, 0 },
	{ "tf_weapon_smg", "SMG", "models/weapons/c_models/c_smg/c_smg.mdl", 2, 1 },
	{ "tf_weapon_club", "Kukri", "models/weapons/c_models/c_machete/c_machete.mdl", 2, 2 },
	// Spy
	{ "tf_weapon_revolver", "Revolver", "models/weapons/c_models/c_revolver/c_revolver.mdl", 8, 0 },
	{ "tf_weapon_knife", "Knife", "models/weapons/c_models/c_knife/c_knife.mdl", 8, 2 },
};
const int g_nKnownWeaponsCount = ARRAYSIZE(g_KnownWeapons);

const char* GetWeaponDisplayName(const char* pszWeaponClass)
{
	for (int i = 0; i < g_nKnownWeaponsCount; i++)
		if (Q_stricmp(g_KnownWeapons[i].pszWeaponClass, pszWeaponClass) == 0)
			return g_KnownWeapons[i].pszDisplayName;
	return pszWeaponClass;
}

const KnownWeaponDef_t* FindWeaponDef(const char* pszWeaponClass)
{
	for (int i = 0; i < g_nKnownWeaponsCount; i++)
		if (Q_stricmp(g_KnownWeapons[i].pszWeaponClass, pszWeaponClass) == 0)
			return &g_KnownWeapons[i];
	return NULL;
}

} // namespace vgui

using namespace vgui;

//-----------------------------------------------------------------------------
// CWeaponModelPreviewPanel
//-----------------------------------------------------------------------------
CWeaponModelPreviewPanel::CWeaponModelPreviewPanel(Panel* parent, const char* panelName)
	: BaseClass(parent, panelName)
	, m_bAutoRotate(true)
	, m_flAutoRotateSpeed(30.0f)
{
	SetLookAtCamera(true);
	
	// Enable full manipulation which includes:
	// Left-click: rotation
	// Middle-click: translate
	// Right-click: zoom (drag up/down)
	// Scroll wheel: zoom
	m_bAllowRotation = true;
	m_bAllowPitch = true;
	m_bAllowFullManipulation = true;
	SetMouseInputEnabled(true);
}

CWeaponModelPreviewPanel::~CWeaponModelPreviewPanel() {}

void CWeaponModelPreviewPanel::SetMDL(MDLHandle_t handle, void *pProxyData)
{
	// Guard against invalid handles - base CMDLPanel crashes without this check
	if (handle == MDLHANDLE_INVALID)
	{
		m_RootMDL.m_MDL.SetMDL(MDLHANDLE_INVALID);
		if (m_RootMDL.m_pStudioHdr)
		{
			delete m_RootMDL.m_pStudioHdr;
			m_RootMDL.m_pStudioHdr = NULL;
		}
		return;
	}
	
	// Verify the studiohdr is valid before letting base class create CStudioHdr
	studiohdr_t *pStudioHdr = g_pMDLCache->GetStudioHdr(handle);
	if (!pStudioHdr)
	{
		Warning("CWeaponModelPreviewPanel::SetMDL - Invalid studio header for handle\n");
		m_RootMDL.m_MDL.SetMDL(MDLHANDLE_INVALID);
		return;
	}
	
	// Safe to call base class now
	BaseClass::SetMDL(handle, pProxyData);
}

void CWeaponModelPreviewPanel::SetMDL(const char *pMDLName, void *pProxyData)
{
	if (!pMDLName || !pMDLName[0])
	{
		SetMDL(MDLHANDLE_INVALID, pProxyData);
		return;
	}
	
	// Find the MDL handle
	MDLHandle_t hMDL = g_pMDLCache->FindMDL(pMDLName);
	if (hMDL == MDLHANDLE_INVALID)
	{
		Warning("CWeaponModelPreviewPanel::SetMDL - FindMDL failed for: %s\n", pMDLName);
		return;
	}
	
	// Check for error model
	if (g_pMDLCache->IsErrorModel(hMDL))
	{
		Warning("CWeaponModelPreviewPanel::SetMDL - Error model for: %s\n", pMDLName);
		g_pMDLCache->Release(hMDL);
		return;
	}
	
	// Manually do what CMDLPanel::SetMDL(handle) does, but safely:
	// 1. Set the MDL handle (this adds a reference)
	m_RootMDL.m_MDL.SetMDL(hMDL);
	
	// 2. Create the CStudioHdr safely
	if (m_RootMDL.m_pStudioHdr)
	{
		delete m_RootMDL.m_pStudioHdr;
		m_RootMDL.m_pStudioHdr = NULL;
	}
	
	studiohdr_t *pStudioHdr = m_RootMDL.m_MDL.GetStudioHdr();
	if (pStudioHdr)
	{
		m_RootMDL.m_pStudioHdr = new CStudioHdr(pStudioHdr, g_pMDLCache);
	}
	
	m_RootMDL.m_MDL.m_pProxyData = pProxyData;
	
	// 3. Setup bounding box and view target
	Vector vecMins, vecMaxs;
	GetMDLBoundingBox(&vecMins, &vecMaxs, hMDL, m_RootMDL.m_MDL.m_nSequence);
	m_RootMDL.m_MDL.m_bWorldSpaceViewTarget = false;
	m_RootMDL.m_MDL.m_vecViewTarget.Init(100.0f, 0.0f, vecMaxs.z);
	m_RootMDL.m_flCycleStartTime = 0.f;
	
	// 4. Release our FindMDL reference (CMDL::SetMDL added its own)
	g_pMDLCache->Release(hMDL);
	
	// 5. Let CBaseModelPanel do its setup (sequence, flex controllers, etc.)
	SetSequence(ACT_IDLE);
	SetupModelDefaults();
	InvalidateLayout();
}

bool CWeaponModelPreviewPanel::LoadModel(const char* pszModelPath)
{
	if (!pszModelPath || !pszModelPath[0])
		return false;
	
	// Check if this is an absolute path - we need to convert it to a relative path
	char szRelativePath[MAX_PATH];
	
	// Try to use the path as-is first (relative path)
	if (g_pFullFileSystem->FileExists(pszModelPath, "GAME"))
	{
		Q_strncpy(szRelativePath, pszModelPath, sizeof(szRelativePath));
	}
	else
	{
		// Try to extract a relative path from an absolute path
		const char* pModelsDir = V_stristr(pszModelPath, "models\\");
		if (!pModelsDir)
			pModelsDir = V_stristr(pszModelPath, "models/");
		
		if (pModelsDir)
		{
			Q_strncpy(szRelativePath, pModelsDir, sizeof(szRelativePath));
			Q_FixSlashes(szRelativePath, '/');
		}
		else
		{
			Warning("CWeaponModelPreviewPanel::LoadModel - Could not find 'models' folder in path: %s\n", pszModelPath);
			return false;
		}
	}
	
	// Verify the model file exists
	if (!g_pFullFileSystem->FileExists(szRelativePath, "GAME"))
	{
		Warning("CWeaponModelPreviewPanel::LoadModel - Model file not found: %s\n", szRelativePath);
		return false;
	}
	
	Msg("CWeaponModelPreviewPanel::LoadModel - Loading: %s\n", szRelativePath);
	
	// Load the model
	SetMDL(szRelativePath);
	
	// Check if model was loaded successfully
	MDLHandle_t hMDL = m_RootMDL.m_MDL.GetMDL();
	Msg("CWeaponModelPreviewPanel::LoadModel - Handle: %d\n", (int)hMDL);
	
	if (hMDL != MDLHANDLE_INVALID)
	{
		ResetCamera();
		return true;
	}
	
	Warning("CWeaponModelPreviewPanel::LoadModel - Model handle is invalid after SetMDL\n");
	return false;
}

void CWeaponModelPreviewPanel::ClearModel()
{
	// Use empty string which base class handles
	SetMDL((const char*)NULL);
}

void CWeaponModelPreviewPanel::ResetCamera()
{
	// Reset all rotation and position
	m_angPlayer.Init();
	m_vecPlayerPos.Init();
	
	// Reset the model pose
	m_BMPResData.m_angModelPoseRot.Init();
	m_BMPResData.m_vecOriginOffset.Init();
	m_BMPResData.m_vecFramedOriginOffset.Init();
	m_BMPResData.m_vecViewportOffset.Init();
	
	// Re-center camera on model
	LookAtMDL();
}

void CWeaponModelPreviewPanel::SetAutoRotate(bool b) { m_bAutoRotate = b; }

void CWeaponModelPreviewPanel::OnThink()
{
	BaseClass::OnThink();
	
	// Auto-rotate when enabled and not being manipulated
	if (m_bAutoRotate && !m_bMousePressed)
	{
		RotateYaw(gpGlobals->frametime * m_flAutoRotateSpeed);
	}
}

void CWeaponModelPreviewPanel::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	SetPaintBackgroundEnabled(true);
	SetPaintBackgroundType(0);
	SetBackgroundColor(Color(30, 28, 27, 255));
}

void CWeaponModelPreviewPanel::PaintBackground()
{
	int wide, tall;
	GetSize(wide, tall);
	
	// Draw solid dark background
	surface()->DrawSetColor(Color(30, 28, 27, 255));
	surface()->DrawFilledRect(0, 0, wide, tall);
}

void CWeaponModelPreviewPanel::OnMousePressed(MouseCode code)
{
	// Let base class handle all mouse input - it supports:
	// Left-click: rotation
	// Middle-click: translate
	// Right-click: zoom
	BaseClass::OnMousePressed(code);
}

void CWeaponModelPreviewPanel::OnMouseReleased(MouseCode code)
{
	BaseClass::OnMouseReleased(code);
}

void CWeaponModelPreviewPanel::OnCursorMoved(int x, int y)
{
	BaseClass::OnCursorMoved(x, y);
}

void CWeaponModelPreviewPanel::OnMouseWheeled(int delta)
{
	// Use base class zoom - requires m_bAllowFullManipulation to be true
	BaseClass::OnMouseWheeled(delta);
}

//-----------------------------------------------------------------------------
// CCFWorkshopWeaponTool
//-----------------------------------------------------------------------------
CCFWorkshopWeaponTool::CCFWorkshopWeaponTool(Panel* parent, const char* panelName)
	: BaseClass(parent, panelName)
	, m_eBrowseType(BROWSE_MODEL)
	, m_pModelPathEntry(NULL)
	, m_pBrowseModelButton(NULL)
	, m_pModelPreview(NULL)
	, m_pModelInfoLabel(NULL)
	, m_pAutoRotateCheck(NULL)
	, m_pClassFilterCombo(NULL)
	, m_pWeaponCombo(NULL)
	, m_pReplaceViewModelCheck(NULL)
	, m_pReplaceWorldModelCheck(NULL)
	, m_pAddReplacementButton(NULL)
	, m_pRemoveReplacementButton(NULL)
	, m_pReplacementList(NULL)
	, m_pModNameEntry(NULL)
	, m_pModDescriptionEntry(NULL)
	, m_pOutputPathEntry(NULL)
	, m_pBrowseOutputButton(NULL)
	, m_pGenerateButton(NULL)
	, m_pCancelButton(NULL)
	, m_pStatusLabel(NULL)
	, m_flNextStatusClearTime(0)
{
	SetScheme(scheme()->LoadSchemeFromFileEx(enginevgui->GetPanel(PANEL_CLIENTDLL), 
		"resource/ClientScheme.res", "ClientScheme"));
	SetProportional(false);
	SetDeleteSelfOnClose(true);
	SetSizeable(false);
	SetMoveable(true);
    SetCloseButtonVisible(true);
    SetTitleBarVisible(false);
	SetSize(1200, 900);
	MoveToCenterOfScreen();
}

CCFWorkshopWeaponTool::~CCFWorkshopWeaponTool() {}

void CCFWorkshopWeaponTool::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	LoadControlSettings("resource/UI/WorkshopWeaponTool.res");
	
	// Find controls
	m_pModelPathEntry = dynamic_cast<TextEntry*>(FindChildByName("ModelPathEntry"));
	m_pBrowseModelButton = dynamic_cast<Button*>(FindChildByName("BrowseModelButton"));
	
	// Create the model preview panel inside the placeholder container
	Panel* pPreviewContainer = FindChildByName("ModelPreviewPanel");
	if (pPreviewContainer && !m_pModelPreview)
	{
		int w, h;
		pPreviewContainer->GetSize(w, h);
		
		// Parent to the container so it draws inside it
		m_pModelPreview = new CWeaponModelPreviewPanel(pPreviewContainer, "ModelPreview");
		m_pModelPreview->SetBounds(0, 0, w, h);
		m_pModelPreview->SetVisible(true);
	}
	m_pModelInfoLabel = dynamic_cast<Label*>(FindChildByName("ModelInfoLabel"));
	m_pAutoRotateCheck = dynamic_cast<CheckButton*>(FindChildByName("AutoRotateCheck"));
	m_pClassFilterCombo = dynamic_cast<ComboBox*>(FindChildByName("ClassFilterCombo"));
	m_pWeaponCombo = dynamic_cast<ComboBox*>(FindChildByName("WeaponCombo"));
	m_pReplaceViewModelCheck = dynamic_cast<CheckButton*>(FindChildByName("ReplaceViewModelCheck"));
	m_pReplaceWorldModelCheck = dynamic_cast<CheckButton*>(FindChildByName("ReplaceWorldModelCheck"));
	m_pAddReplacementButton = dynamic_cast<Button*>(FindChildByName("AddReplacementButton"));
	m_pRemoveReplacementButton = dynamic_cast<Button*>(FindChildByName("RemoveReplacementButton"));
	m_pReplacementList = dynamic_cast<ListPanel*>(FindChildByName("ReplacementList"));
	m_pModNameEntry = dynamic_cast<TextEntry*>(FindChildByName("ModNameEntry"));
	m_pModDescriptionEntry = dynamic_cast<TextEntry*>(FindChildByName("ModDescriptionEntry"));
	m_pOutputPathEntry = dynamic_cast<TextEntry*>(FindChildByName("OutputPathEntry"));
	m_pBrowseOutputButton = dynamic_cast<Button*>(FindChildByName("BrowseOutputButton"));
	m_pGenerateButton = dynamic_cast<Button*>(FindChildByName("GenerateButton"));
	m_pCancelButton = dynamic_cast<Button*>(FindChildByName("CancelButton"));
	m_pStatusLabel = dynamic_cast<Label*>(FindChildByName("StatusLabel"));
	
	// Setup class filter combo
	if (m_pClassFilterCombo) {
		m_pClassFilterCombo->AddItem("All Classes", NULL);
		m_pClassFilterCombo->AddItem("Scout", NULL);
		m_pClassFilterCombo->AddItem("Sniper", NULL);
		m_pClassFilterCombo->AddItem("Soldier", NULL);
		m_pClassFilterCombo->AddItem("Demoman", NULL);
		m_pClassFilterCombo->AddItem("Medic", NULL);
		m_pClassFilterCombo->AddItem("Heavy", NULL);
		m_pClassFilterCombo->AddItem("Pyro", NULL);
		m_pClassFilterCombo->AddItem("Spy", NULL);
		m_pClassFilterCombo->AddItem("Engineer", NULL);
		m_pClassFilterCombo->ActivateItemByRow(0);
		m_pClassFilterCombo->AddActionSignalTarget(this);
	}
	
	// Setup replacement list columns
	if (m_pReplacementList) {
		m_pReplacementList->AddColumnHeader(0, "weapon", "Target Weapon", 200, ListPanel::COLUMN_RESIZEWITHWINDOW);
		m_pReplacementList->AddColumnHeader(1, "model", "Custom Model", 250, ListPanel::COLUMN_RESIZEWITHWINDOW);
		m_pReplacementList->AddColumnHeader(2, "type", "Type", 80, 0);
	}
	
	// Default checkboxes
	if (m_pReplaceViewModelCheck) m_pReplaceViewModelCheck->SetSelected(true);
	if (m_pReplaceWorldModelCheck) m_pReplaceWorldModelCheck->SetSelected(true);
	if (m_pAutoRotateCheck) m_pAutoRotateCheck->SetSelected(true);
	
	PopulateWeaponDropdown();
}

void CCFWorkshopWeaponTool::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CCFWorkshopWeaponTool::PopulateWeaponDropdown()
{
	if (!m_pWeaponCombo) return;
	m_pWeaponCombo->RemoveAll();
	
	int classFilter = m_pClassFilterCombo ? m_pClassFilterCombo->GetActiveItem() : 0;
	
	for (int i = 0; i < g_nKnownWeaponsCount; i++) {
		if (classFilter == 0 || g_KnownWeapons[i].iClassIndex == classFilter)
			m_pWeaponCombo->AddItem(g_KnownWeapons[i].pszDisplayName, new KeyValues("data", "class", g_KnownWeapons[i].pszWeaponClass));
	}
	if (m_pWeaponCombo->GetItemCount() > 0)
		m_pWeaponCombo->ActivateItemByRow(0);
}

void CCFWorkshopWeaponTool::FilterWeaponsByClass(int iClassIndex)
{
	PopulateWeaponDropdown();
}

void CCFWorkshopWeaponTool::OnCommand(const char* command)
{
	if (Q_stricmp(command, "BrowseModel") == 0) {
		OpenModelFileBrowser();
	} else if (Q_stricmp(command, "BrowseOutput") == 0) {
		OpenOutputDirectoryBrowser();
	} else if (Q_stricmp(command, "AddWeapon") == 0 || Q_stricmp(command, "AddReplacement") == 0) {
		AddModelReplacement();
	} else if (Q_stricmp(command, "RemoveWeapon") == 0 || Q_stricmp(command, "RemoveReplacement") == 0) {
		RemoveSelectedReplacement();
	} else if (Q_stricmp(command, "CreateMod") == 0 || Q_stricmp(command, "Generate") == 0) {
		GenerateWeaponMod();
	} else if (Q_stricmp(command, "ClearList") == 0) {
		ClearAllReplacements();
	} else if (Q_stricmp(command, "ResetView") == 0) {
		if (m_pModelPreview)
			m_pModelPreview->ResetCamera();
	} else if (Q_stricmp(command, "Cancel") == 0 || Q_stricmp(command, "Close") == 0) {
		Close();
	} else if (Q_stricmp(command, "ClassFilterChanged") == 0) {
		PopulateWeaponDropdown();
	} else if (Q_stricmp(command, "ToggleAutoRotate") == 0) {
		if (m_pModelPreview && m_pAutoRotateCheck)
			m_pModelPreview->SetAutoRotate(m_pAutoRotateCheck->IsSelected());
	} else {
		BaseClass::OnCommand(command);
	}
}

void CCFWorkshopWeaponTool::OnThink()
{
	BaseClass::OnThink();
	if (m_flNextStatusClearTime > 0 && gpGlobals->curtime > m_flNextStatusClearTime) {
		if (m_pStatusLabel) m_pStatusLabel->SetText("");
		m_flNextStatusClearTime = 0;
	}
}

void CCFWorkshopWeaponTool::OnKeyCodePressed(KeyCode code)
{
	if (code == KEY_ESCAPE) Close();
	else BaseClass::OnKeyCodePressed(code);
}

void CCFWorkshopWeaponTool::ShowPanel(bool bShow)
{
	SetVisible(bShow);
	if (bShow) { MoveToFront(); RequestFocus(); }
}

void CCFWorkshopWeaponTool::OpenModelFileBrowser()
{
	m_eBrowseType = BROWSE_MODEL;
	FileOpenDialog* pDialog = new FileOpenDialog(this, "Select Model File", true);
	pDialog->AddFilter("*.mdl", "Model Files (*.mdl)", true);
	pDialog->AddActionSignalTarget(this);
	pDialog->DoModal();
}

void CCFWorkshopWeaponTool::OpenOutputDirectoryBrowser()
{
	m_eBrowseType = BROWSE_OUTPUT;
	DirectorySelectDialog* pDialog = new DirectorySelectDialog(this, "Select Output Directory");
	pDialog->AddActionSignalTarget(this);
	pDialog->MakeReadyForUse();
	
	// Set start directory to current output path or current directory
	char szStartDir[MAX_PATH];
	if (m_strOutputPath.Length() > 0)
	{
		Q_strncpy(szStartDir, m_strOutputPath.Get(), sizeof(szStartDir));
	}
	else
	{
		g_pFullFileSystem->GetCurrentDirectory(szStartDir, sizeof(szStartDir));
	}
	pDialog->SetStartDirectory(szStartDir);
	pDialog->DoModal();
}

void CCFWorkshopWeaponTool::OnFileSelected(KeyValues* params)
{
	const char* pszPath = params->GetString("fullpath");
	if (!pszPath || !pszPath[0]) return;
	
	if (m_eBrowseType == BROWSE_MODEL) {
		m_strCurrentModelPath = pszPath;
		if (m_pModelPathEntry) m_pModelPathEntry->SetText(pszPath);
		UpdateModelPreview();
	}
}

void CCFWorkshopWeaponTool::OnDirectorySelected(KeyValues* params)
{
	const char* pszPath = params->GetString("dir");
	if (!pszPath || !pszPath[0]) return;
	
	m_strOutputPath = pszPath;
	if (m_pOutputPathEntry) m_pOutputPathEntry->SetText(pszPath);
}

void CCFWorkshopWeaponTool::OnItemSelected() {}
void CCFWorkshopWeaponTool::OnItemDoubleClicked(int itemID) {}

void CCFWorkshopWeaponTool::UpdateModelPreview()
{
	if (!m_pModelPreview || m_strCurrentModelPath.IsEmpty()) return;
	
	const char* pszPath = m_strCurrentModelPath.Get();
	
	// Check if this is a valid model path
	const char* pModelsDir = V_stristr(pszPath, "models\\");
	if (!pModelsDir)
		pModelsDir = V_stristr(pszPath, "models/");
	
	if (!pModelsDir)
	{
		SetStatus("Error: Model must be inside a 'models' folder!");
		return;
	}
	
	if (m_pModelPreview->LoadModel(pszPath)) {
		SetStatus("Model loaded: %s", V_GetFileName(pszPath));
		if (m_pModelInfoLabel)
			m_pModelInfoLabel->SetText(V_GetFileName(pszPath));
	} else {
		SetStatus("Failed to load model - check console for details");
	}
}

void CCFWorkshopWeaponTool::AddModelReplacement()
{
	if (m_strCurrentModelPath.IsEmpty()) {
		SetStatus("Please select a model file first!");
		return;
	}
	if (!m_pWeaponCombo || m_pWeaponCombo->GetActiveItem() < 0) {
		SetStatus("Please select a target weapon!");
		return;
	}
	
	KeyValues* pData = m_pWeaponCombo->GetActiveItemUserData();
	if (!pData) return;
	
	const char* pszWeaponClass = pData->GetString("class");
	const KnownWeaponDef_t* pWeaponDef = FindWeaponDef(pszWeaponClass);
	if (!pWeaponDef) return;
	
	// Add to our data
	WeaponReplacementEntry_t entry;
	entry.strCustomModelPath = m_strCurrentModelPath;
	entry.strTargetWeaponClass = pszWeaponClass;
	entry.strTargetModelPath = pWeaponDef->pszModelPath;
	entry.strDisplayName = pWeaponDef->pszDisplayName;
	entry.bReplaceViewModel = true;
	entry.bReplaceWorldModel = true;
	m_vecReplacements.AddToTail(entry);
	
	// Add to list panel
	if (m_pReplacementList) {
		KeyValues* pKV = new KeyValues("item");
		pKV->SetString("weapon", pWeaponDef->pszDisplayName);
		pKV->SetString("model", V_GetFileName(m_strCurrentModelPath.Get()));
		pKV->SetString("type", "C_Model");
		m_pReplacementList->AddItem(pKV, 0, false, false);
		pKV->deleteThis();
	}
	
	SetStatus("Added replacement: %s -> %s", pWeaponDef->pszDisplayName, V_GetFileName(m_strCurrentModelPath.Get()));
}

void CCFWorkshopWeaponTool::RemoveSelectedReplacement()
{
	if (!m_pReplacementList) return;
	int sel = m_pReplacementList->GetSelectedItem(0);
	if (sel < 0) return;
	
	int dataIdx = m_pReplacementList->GetItemCurrentRow(sel);
	if (dataIdx >= 0 && dataIdx < m_vecReplacements.Count())
		m_vecReplacements.Remove(dataIdx);
	
	m_pReplacementList->RemoveItem(sel);
	SetStatus("Replacement removed");
}

void CCFWorkshopWeaponTool::ClearAllReplacements()
{
	m_vecReplacements.RemoveAll();
	if (m_pReplacementList) m_pReplacementList->RemoveAll();
}

void CCFWorkshopWeaponTool::SetStatus(const char* pszFormat, ...)
{
	char szBuffer[512];
	va_list args;
	va_start(args, pszFormat);
	V_vsnprintf(szBuffer, sizeof(szBuffer), pszFormat, args);
	va_end(args);
	
	if (m_pStatusLabel) m_pStatusLabel->SetText(szBuffer);
	m_flNextStatusClearTime = gpGlobals->curtime + 5.0f;
}

bool CCFWorkshopWeaponTool::ValidateModelFile(const char* pszModelPath)
{
	return pszModelPath && g_pFullFileSystem->FileExists(pszModelPath);
}

void CCFWorkshopWeaponTool::GenerateWeaponMod()
{
	if (m_vecReplacements.Count() == 0) {
		SetStatus("No weapon replacements defined!");
		return;
	}
	
	if (m_strOutputPath.IsEmpty()) {
		SetStatus("Please select an output directory!");
		return;
	}
	
	int nSuccessCount = 0;
	
	// Process each replacement
	for (int i = 0; i < m_vecReplacements.Count(); i++)
	{
		const WeaponReplacementEntry_t& entry = m_vecReplacements[i];
		const KnownWeaponDef_t* pWeaponDef = FindWeaponDef(entry.strTargetWeaponClass.Get());
		if (!pWeaponDef || !pWeaponDef->pszModelPath)
			continue;
		
		// Copy to c_model path
		if (CopyModelToTarget(entry.strCustomModelPath.Get(), pWeaponDef->pszModelPath))
			nSuccessCount++;
	}
	
	if (nSuccessCount > 0)
	{
		SetStatus("Created %d model replacement(s) at: %s", nSuccessCount, m_strOutputPath.Get());
		
		// Show success message
		char szMsg[512];
		V_snprintf(szMsg, sizeof(szMsg), "Successfully created %d model file(s)!\n\nOutput: %s", nSuccessCount, m_strOutputPath.Get());
		MessageBox* pMsg = new MessageBox("Success", szMsg, this);
		pMsg->DoModal();
	}
	else
	{
		SetStatus("Failed to create any model files!");
	}
}

bool CCFWorkshopWeaponTool::CopyModelToTarget(const char* pszSourcePath, const char* pszTargetModelPath)
{
	if (!pszSourcePath || !pszSourcePath[0] || !pszTargetModelPath || !pszTargetModelPath[0])
		return false;
	
	// Clean up the output path - remove trailing slashes
	char szOutputDir[MAX_PATH];
	V_strcpy_safe(szOutputDir, m_strOutputPath.Get());
	V_StripTrailingSlash(szOutputDir);
	
	// Get source and dest base paths (without .mdl extension)
	char szSourceBase[MAX_PATH];
	char szDestBase[MAX_PATH];
	
	V_strcpy_safe(szSourceBase, pszSourcePath);
	V_StripExtension(szSourceBase, szSourceBase, sizeof(szSourceBase));
	
	V_snprintf(szDestBase, sizeof(szDestBase), "%s%c%s", szOutputDir, CORRECT_PATH_SEPARATOR, pszTargetModelPath);
	V_StripExtension(szDestBase, szDestBase, sizeof(szDestBase));
	V_FixSlashes(szDestBase);
	
	// Create the directory hierarchy for the destination
	char szDestDir[MAX_PATH];
	V_strcpy_safe(szDestDir, szDestBase);
	V_StripFilename(szDestDir);
	
	// Create directories recursively
	CUtlVector<CUtlString> dirsToCreate;
	char szCheckDir[MAX_PATH];
	V_strcpy_safe(szCheckDir, szDestDir);
	
	while (szCheckDir[0] && !g_pFullFileSystem->IsDirectory(szCheckDir))
	{
		dirsToCreate.AddToHead(szCheckDir);
		V_StripLastDir(szCheckDir, sizeof(szCheckDir));
		V_StripTrailingSlash(szCheckDir);
	}
	
	for (int i = 0; i < dirsToCreate.Count(); i++)
	{
		_mkdir(dirsToCreate[i].Get());
	}
	
	// Get source full path base
	char szSourceFullBase[MAX_PATH];
	if (V_IsAbsolutePath(szSourceBase))
	{
		V_strcpy_safe(szSourceFullBase, szSourceBase);
	}
	else
	{
		char szTemp[MAX_PATH];
		V_snprintf(szTemp, sizeof(szTemp), "%s.mdl", szSourceBase);
		g_pFullFileSystem->RelativePathToFullPath(szTemp, "GAME", szSourceFullBase, sizeof(szSourceFullBase));
		V_StripExtension(szSourceFullBase, szSourceFullBase, sizeof(szSourceFullBase));
	}
	V_FixSlashes(szSourceFullBase);
	
	// List of model file extensions to copy
	const char* szExtensions[] = {
		".mdl",
		".vvd",
		".phy",
		".dx90.vtx",
		".dx80.vtx",
		".sw.vtx",
	};
	
	int nFilesCopied = 0;
	
	for (int i = 0; i < ARRAYSIZE(szExtensions); i++)
	{
		char szSrcFile[MAX_PATH];
		char szDstFile[MAX_PATH];
		
		V_snprintf(szSrcFile, sizeof(szSrcFile), "%s%s", szSourceFullBase, szExtensions[i]);
		V_snprintf(szDstFile, sizeof(szDstFile), "%s%s", szDestBase, szExtensions[i]);
		
		// Check if source file exists
		FILE* pSrcFile = fopen(szSrcFile, "rb");
		if (!pSrcFile)
		{
			// .phy is optional, others should warn
			if (Q_stricmp(szExtensions[i], ".phy") != 0)
			{
				Warning("CopyModelToTarget: Source file not found: %s\n", szSrcFile);
			}
			continue;
		}
		
		// Get file size
		fseek(pSrcFile, 0, SEEK_END);
		long nFileSize = ftell(pSrcFile);
		fseek(pSrcFile, 0, SEEK_SET);
		
		// Read file contents
		unsigned char* pBuffer = new unsigned char[nFileSize];
		size_t nRead = fread(pBuffer, 1, nFileSize, pSrcFile);
		fclose(pSrcFile);
		
		if (nRead != (size_t)nFileSize)
		{
			delete[] pBuffer;
			Warning("CopyModelToTarget: Failed to read file: %s\n", szSrcFile);
			continue;
		}
		
		// Write to destination
		FILE* pDstFile = fopen(szDstFile, "wb");
		if (!pDstFile)
		{
			delete[] pBuffer;
			Warning("CopyModelToTarget: Failed to create file: %s\n", szDstFile);
			continue;
		}
		
		size_t nWritten = fwrite(pBuffer, 1, nFileSize, pDstFile);
		fclose(pDstFile);
		delete[] pBuffer;
		
		if (nWritten == (size_t)nFileSize)
		{
			Msg("CopyModelToTarget: Copied %s\n", szExtensions[i]);
			nFilesCopied++;
		}
	}
	
	Msg("CopyModelToTarget: Copied %d files for %s\n", nFilesCopied, V_GetFileName(pszTargetModelPath));
	return nFilesCopied > 0;
}

bool CCFWorkshopWeaponTool::CopyModelFiles(const char* pszSourcePath, const char* pszDestPath)
{
	// This function is now deprecated - use CopyModelToTarget instead
	return false;
}

bool CCFWorkshopWeaponTool::GenerateWeaponScript(const char* pszOutputPath)
{
	// Disabled for now
	return true;
}

//-----------------------------------------------------------------------------
// Console command
//-----------------------------------------------------------------------------
CON_COMMAND(cf_workshop_weapon_tool, "Open the Workshop Weapon Mod Tool")
{
	CCFWorkshopWeaponTool* pTool = new CCFWorkshopWeaponTool(NULL, "WorkshopWeaponTool");
	pTool->ShowPanel(true);
	pTool->MakePopup();
}

void OpenWorkshopWeaponTool()
{
	engine->ClientCmd_Unrestricted("cf_workshop_weapon_tool");
}

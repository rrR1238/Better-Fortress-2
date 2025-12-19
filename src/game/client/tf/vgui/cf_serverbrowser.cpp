//========= Copyright Custom Fortress 2, All rights reserved. ============//
//
// Purpose: Custom Server Browser MK.IV Implementation
//
//=============================================================================//

#include "cbase.h"
#include "cf_serverbrowser.h"
#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ToggleButton.h>
#include "ienginevgui.h"
#include "engine/IEngineSound.h"
#include "steam/steam_api.h"
#include "filesystem.h"
#include "tier3/tier3.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

CCFServerBrowser *CCFServerBrowser::s_pInstance = NULL;

ConVar cf_serverbrowser_refreshrate( "cf_serverbrowser_refreshrate", "5", FCVAR_ARCHIVE, "Server browser auto-refresh interval" );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCFServerBrowser::CCFServerBrowser( VPANEL parent ) : BaseClass( NULL, "CFServerBrowser" )
{
	SetParent( parent );
	
	SetKeyBoardInputEnabled( true );
	SetMouseInputEnabled( true );
	
	SetProportional( false );
	SetTitleBarVisible( true );
	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetCloseButtonVisible( true );
	SetSizeable( true );
	SetMoveable( true );
	SetVisible( false );
	
	SetScheme( "ClientScheme" );
	
	m_pTitleLabel = new Label( this, "TitleLabel", "SERVER BROWSER MK.IV" );
	m_pGameTabs = new PropertySheet( this, "GameTabs" );
	m_pGameTabs->SetTabWidth( 100 );
	
	// Add tabs
	m_pGameTabs->AddPage( new CInternetGamesPage( m_pGameTabs ), "Internet" );
	m_pGameTabs->AddPage( new CFavoriteGamesPage( m_pGameTabs ), "Favorite" );
	m_pGameTabs->AddPage( new CHistoryGamesPage( m_pGameTabs ), "History" );
	m_pGameTabs->AddPage( new CFriendsGamesPage( m_pGameTabs ), "Friends" );
	m_pGameTabs->AddPage( new CSpectateGamesPage( m_pGameTabs ), "Spectate" );
	m_pGameTabs->AddPage( new CLanGamesPage( m_pGameTabs ), "Lan" );
	m_pGameTabs->AddPage( new CBlacklistedServersPage( m_pGameTabs ), "Blacklisted Servers" );
	
	LoadControlSettings( "Resource/UI/ServerBrowser/ServerBrowser.res" );
	
	s_pInstance = this;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CCFServerBrowser::~CCFServerBrowser()
{
	s_pInstance = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Get singleton instance
//-----------------------------------------------------------------------------
CCFServerBrowser *CCFServerBrowser::GetInstance()
{
	if ( !s_pInstance )
	{
		s_pInstance = new CCFServerBrowser( enginevgui->GetPanel( PANEL_GAMEUIDLL ) );
	}
	return s_pInstance;
}

//-----------------------------------------------------------------------------
// Purpose: Show the dialog
//-----------------------------------------------------------------------------
void CCFServerBrowser::ShowDialog()
{
	CCFServerBrowser *pDlg = GetInstance();
	if ( pDlg )
	{
		pDlg->SetVisible( true );
		pDlg->MakePopup();
		pDlg->MoveToFront();
		pDlg->SetKeyBoardInputEnabled( true );
		pDlg->SetMouseInputEnabled( true );
		pDlg->Activate();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Apply scheme settings
//-----------------------------------------------------------------------------
void CCFServerBrowser::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	
	SetBgColor( pScheme->GetColor( "ServerBrowser.Background", Color( 32, 32, 32, 255 ) ) );
	SetTitle( "", false );
	
	if ( m_pTitleLabel )
	{
		m_pTitleLabel->SetFgColor( pScheme->GetColor( "ServerBrowser.TextBright", Color( 200, 200, 200, 255 ) ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Layout
//-----------------------------------------------------------------------------
void CCFServerBrowser::PerformLayout()
{
	BaseClass::PerformLayout();
	
	SetSize( 1200, 700 );
	MoveToCenterOfScreen();
	
	if ( m_pTitleLabel )
	{
		m_pTitleLabel->SetBounds( 20, 10, 400, 30 );
	}
	
	if ( m_pGameTabs )
	{
		m_pGameTabs->SetBounds( 10, 45, GetWide() - 20, GetTall() - 55 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handle commands
//-----------------------------------------------------------------------------
void CCFServerBrowser::OnCommand( const char *command )
{
	if ( !Q_stricmp( command, "Close" ) || !Q_stricmp( command, "vguicancel" ) )
	{
		SetVisible( false );
		MarkForDeletion();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handle key presses
//-----------------------------------------------------------------------------
void CCFServerBrowser::OnKeyCodePressed( KeyCode code )
{
	if ( code == KEY_ESCAPE )
	{
		OnCommand( "Close" );
	}
	else
	{
		BaseClass::OnKeyCodePressed( code );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Activate the dialog
//-----------------------------------------------------------------------------
void CCFServerBrowser::Activate()
{
	BaseClass::Activate();
	
	SetVisible( true );
	MoveToFront();
	RequestFocus();
	
	// Refresh the active tab
	RefreshServerList();
}

//-----------------------------------------------------------------------------
// Purpose: Refresh server list
//-----------------------------------------------------------------------------
void CCFServerBrowser::RefreshServerList()
{
	if ( m_pGameTabs )
	{
		Panel *pActivePage = m_pGameTabs->GetActivePage();
		if ( pActivePage )
		{
			PostMessage( pActivePage, new KeyValues( "RefreshServerList" ) );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Show server info dialog
//-----------------------------------------------------------------------------
void CCFServerBrowser::ShowServerInfo( uint32 serverIP, uint16 serverPort )
{
	CDialogGameInfo *pDlg = new CDialogGameInfo( this );
	pDlg->UpdateServerInfo( serverIP, serverPort );
	pDlg->Activate();
}

//-----------------------------------------------------------------------------
// Purpose: Server selected
//-----------------------------------------------------------------------------
void CCFServerBrowser::OnServerSelected()
{
	// Handled by individual pages
}

//-----------------------------------------------------------------------------
// Purpose: Internet Games Page Constructor
//-----------------------------------------------------------------------------
CInternetGamesPage::CInternetGamesPage( Panel *parent ) : BaseClass( parent, "InternetGames" )
{
	m_pGameList = new ListPanel( this, "GameList" );
	m_pConnectButton = new Button( this, "ConnectButton", "#ServerBrowser_Connect" );
	m_pRefreshButton = new Button( this, "RefreshButton", "#ServerBrowser_Refresh" );
	m_pServerInfoButton = new Button( this, "ServerInfoButton", "#ServerBrowser_ServerInfo" );
	m_pGameFilter = new ComboBox( this, "GameFilter", 5, false );
	m_pLocationFilter = new ComboBox( this, "LocationFilter", 5, false );
	m_pMapFilter = new TextEntry( this, "MapFilter" );
	
	m_hServerListRequest = NULL;
	m_bRefreshing = false;
	
	// Setup list columns
	m_pGameList->AddColumnHeader( 0, "map", "#ServerBrowser_MapName", 100, ListPanel::COLUMN_FIXEDSIZE | ListPanel::COLUMN_IMAGE );
	m_pGameList->AddColumnHeader( 1, "password", "", 20, ListPanel::COLUMN_FIXEDSIZE | ListPanel::COLUMN_IMAGE );
	m_pGameList->AddColumnHeader( 2, "name", "#ServerBrowser_Servers", 300, ListPanel::COLUMN_RESIZEWITHWINDOW );
	m_pGameList->AddColumnHeader( 3, "players", "#ServerBrowser_Players", 60, ListPanel::COLUMN_FIXEDSIZE );
	m_pGameList->AddColumnHeader( 4, "ping", "#ServerBrowser_Latency", 50, ListPanel::COLUMN_FIXEDSIZE );
	m_pGameList->AddColumnHeader( 5, "tags", "#ServerBrowser_Tags", 200, ListPanel::COLUMN_FIXEDSIZE );
	
	m_pGameList->SetSortColumn( 4 ); // Sort by ping
	m_pGameList->SetSortFunc( 4, [] ( vgui::ListPanel *pPanel, const ListPanelItem &item1, const ListPanelItem &item2 ) -> int {
		int ping1 = item1.kv->GetInt( "ping" );
		int ping2 = item2.kv->GetInt( "ping" );
		return ping1 - ping2;
	});
	
	m_pConnectButton->SetCommand( "Connect" );
	m_pRefreshButton->SetCommand( "GetNewList" );
	m_pServerInfoButton->SetCommand( "ServerInfo" );
	
	m_pConnectButton->SetEnabled( false );
	m_pServerInfoButton->SetEnabled( false );
	
	// Add filters
	m_pGameFilter->AddItem( "All Games", NULL );
	m_pGameFilter->AddItem( "Custom Fortress", NULL );
	m_pGameFilter->ActivateItemByRow( 0 );
	
	m_pLocationFilter->AddItem( "All Regions", NULL );
	m_pLocationFilter->AddItem( "NA", NULL );
	m_pLocationFilter->AddItem( "SA", NULL );
	m_pLocationFilter->AddItem( "EU", NULL );
	m_pLocationFilter->AddItem( "AS", NULL );
	m_pLocationFilter->AddItem( "AU", NULL );
	m_pLocationFilter->ActivateItemByRow( 0 );
	
	LoadControlSettings( "Resource/UI/ServerBrowser/InternetGames.res" );
	
	ivgui()->AddTickSignal( GetVPanel(), 100 );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CInternetGamesPage::~CInternetGamesPage()
{
	if ( m_hServerListRequest && steamapicontext && steamapicontext->SteamMatchmakingServers() )
	{
		steamapicontext->SteamMatchmakingServers()->ReleaseRequest( m_hServerListRequest );
		m_hServerListRequest = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Apply scheme settings
//-----------------------------------------------------------------------------
void CInternetGamesPage::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	
	SetBgColor( pScheme->GetColor( "ServerBrowser.Panel", Color( 40, 40, 40, 255 ) ) );
	
	if ( m_pGameList )
	{
		m_pGameList->SetBgColor( pScheme->GetColor( "ServerBrowser.Panel", Color( 40, 40, 40, 255 ) ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Layout
//-----------------------------------------------------------------------------
void CInternetGamesPage::PerformLayout()
{
	BaseClass::PerformLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Handle commands
//-----------------------------------------------------------------------------
void CInternetGamesPage::OnCommand( const char *command )
{
	if ( !Q_stricmp( command, "Connect" ) )
	{
		ConnectToSelectedServer();
	}
	else if ( !Q_stricmp( command, "GetNewList" ) )
	{
		RefreshServerList();
	}
	else if ( !Q_stricmp( command, "ServerInfo" ) )
	{
		ShowServerInfo();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Item selected
//-----------------------------------------------------------------------------
void CInternetGamesPage::OnItemSelected( KeyValues *kv )
{
	int itemID = m_pGameList->GetSelectedItem( 0 );
	bool bSelected = ( itemID >= 0 );
	
	m_pConnectButton->SetEnabled( bSelected );
	m_pServerInfoButton->SetEnabled( bSelected );
}

//-----------------------------------------------------------------------------
// Purpose: Refresh complete callback
//-----------------------------------------------------------------------------
void CInternetGamesPage::OnRefreshComplete( HServerListRequest hRequest, EMatchMakingServerResponse response )
{
	m_bRefreshing = false;
	if ( m_pRefreshButton )
	{
		m_pRefreshButton->SetEnabled( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Refresh server list
//-----------------------------------------------------------------------------
void CInternetGamesPage::RefreshServerList()
{
	if ( !steamapicontext || !steamapicontext->SteamMatchmakingServers() )
		return;
	
	m_pGameList->RemoveAll();
	m_bRefreshing = true;
	
	if ( m_pRefreshButton )
	{
		m_pRefreshButton->SetEnabled( false );
	}
	
	// Request internet servers
	MatchMakingKeyValuePair_t *pFilters[1];
	MatchMakingKeyValuePair_t filter;
	V_strcpy_safe( filter.m_szKey, "gamedir" );
	V_strcpy_safe( filter.m_szValue, "customfortress" );
	pFilters[0] = &filter;
	
	if ( m_hServerListRequest )
	{
		steamapicontext->SteamMatchmakingServers()->ReleaseRequest( m_hServerListRequest );
	}
	
	// Note: We need to implement ISteamMatchmakingServerListResponse callbacks
	// This is a simplified version - full implementation would use callbacks
	m_hServerListRequest = steamapicontext->SteamMatchmakingServers()->RequestInternetServerList( 
		steamapicontext->SteamUtils()->GetAppID(), 
		pFilters, 
		1, 
		NULL  // We'd need to pass a callback handler here
	);
}

//-----------------------------------------------------------------------------
// Purpose: Connect to selected server
//-----------------------------------------------------------------------------
void CInternetGamesPage::ConnectToSelectedServer()
{
	int itemID = m_pGameList->GetSelectedItem( 0 );
	if ( itemID < 0 )
		return;
	
	KeyValues *kv = m_pGameList->GetItem( itemID );
	if ( !kv )
		return;
	
	uint32 serverIP = kv->GetInt( "serverip" );
	uint16 serverPort = kv->GetInt( "serverport" );
	
	char cmd[256];
	Q_snprintf( cmd, sizeof( cmd ), "connect %d.%d.%d.%d:%d",
		( serverIP >> 24 ) & 0xFF,
		( serverIP >> 16 ) & 0xFF,
		( serverIP >> 8 ) & 0xFF,
		serverIP & 0xFF,
		serverPort );
	
	engine->ClientCmd_Unrestricted( cmd );
}

//-----------------------------------------------------------------------------
// Purpose: Show server info
//-----------------------------------------------------------------------------
void CInternetGamesPage::ShowServerInfo()
{
	int itemID = m_pGameList->GetSelectedItem( 0 );
	if ( itemID < 0 )
		return;
	
	KeyValues *kv = m_pGameList->GetItem( itemID );
	if ( !kv )
		return;
	
	uint32 serverIP = kv->GetInt( "serverip" );
	uint16 serverPort = kv->GetInt( "serverport" );
	
	CCFServerBrowser *pBrowser = CCFServerBrowser::GetInstance();
	if ( pBrowser )
	{
		pBrowser->ShowServerInfo( serverIP, serverPort );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Server responded
//-----------------------------------------------------------------------------
void CInternetGamesPage::OnServerResponded( gameserveritem_t &server )
{
	KeyValues *kv = new KeyValues( "server" );
	
	kv->SetString( "name", server.GetName() );
	kv->SetString( "map", server.m_szMap );
	kv->SetInt( "players", server.m_nPlayers );
	kv->SetInt( "maxplayers", server.m_nMaxPlayers );
	kv->SetInt( "ping", server.m_nPing );
	kv->SetString( "tags", server.m_szGameTags );
	kv->SetInt( "serverip", server.m_NetAdr.GetIP() );
	kv->SetInt( "serverport", server.m_NetAdr.GetConnectionPort() );
	kv->SetInt( "password", server.m_bPassword ? 1 : 0 );
	
	char szPlayers[32];
	Q_snprintf( szPlayers, sizeof( szPlayers ), "%d/%d", server.m_nPlayers, server.m_nMaxPlayers );
	kv->SetString( "players", szPlayers );
	
	m_pGameList->AddItem( kv, 0, false, false );
	kv->deleteThis();
}

//-----------------------------------------------------------------------------
// Derived page constructors
//-----------------------------------------------------------------------------
CFavoriteGamesPage::CFavoriteGamesPage( Panel *parent ) : BaseClass( parent )
{
	SetName( "FavoriteGames" );
	LoadControlSettings( "Resource/UI/ServerBrowser/FavoriteGames.res" );
}

void CFavoriteGamesPage::RefreshServerList()
{
	// TODO: Query favorite servers from Steam
	BaseClass::RefreshServerList();
}

CHistoryGamesPage::CHistoryGamesPage( Panel *parent ) : BaseClass( parent )
{
	SetName( "HistoryGames" );
	LoadControlSettings( "Resource/UI/ServerBrowser/HistoryGames.res" );
}

void CHistoryGamesPage::RefreshServerList()
{
	// TODO: Query history servers
	BaseClass::RefreshServerList();
}

CFriendsGamesPage::CFriendsGamesPage( Panel *parent ) : BaseClass( parent )
{
	SetName( "FriendsGames" );
	LoadControlSettings( "Resource/UI/ServerBrowser/FriendsGames.res" );
}

void CFriendsGamesPage::RefreshServerList()
{
	// TODO: Query servers with friends
	BaseClass::RefreshServerList();
}

CLanGamesPage::CLanGamesPage( Panel *parent ) : BaseClass( parent )
{
	SetName( "LanGames" );
	LoadControlSettings( "Resource/UI/ServerBrowser/LanGames.res" );
}

void CLanGamesPage::RefreshServerList()
{
	// TODO: Query LAN servers
	if ( !steamapicontext || !steamapicontext->SteamMatchmakingServers() )
		return;
	
	m_pGameList->RemoveAll();
	// Request LAN servers instead
}

CSpectateGamesPage::CSpectateGamesPage( Panel *parent ) : BaseClass( parent )
{
	SetName( "SpectateGames" );
	LoadControlSettings( "Resource/UI/ServerBrowser/SpectateGames.res" );
}

void CSpectateGamesPage::RefreshServerList()
{
	// TODO: Query spectatable servers
	BaseClass::RefreshServerList();
}

//-----------------------------------------------------------------------------
// Purpose: Dialog Game Info Constructor
//-----------------------------------------------------------------------------
CDialogGameInfo::CDialogGameInfo( Panel *parent ) : BaseClass( NULL, "DialogGameInfo" )
{
	SetParent( parent );
	SetDeleteSelfOnClose( true );
	
	SetKeyBoardInputEnabled( true );
	SetMouseInputEnabled( true );
	
	SetProportional( false );
	SetTitleBarVisible( true );
	SetMinimizeButtonVisible( false );
	SetMaximizeButtonVisible( false );
	SetCloseButtonVisible( true );
	SetSizeable( false );
	SetMoveable( true );
	SetVisible( false );
	
	SetScheme( "ClientScheme" );
	
	m_pMapImage = new ImagePanel( this, "MapImage" );
	m_pServerName = new Label( this, "ServerName", "" );
	m_pServerStatusLabel = new Label( this, "ServerStatusLabel", "" );
	m_pMapName = new Label( this, "MapName", "" );
	m_pGameTags = new Label( this, "GameTags", "" );
	m_pPlayerCountLabel = new Label( this, "PlayerCountLabel", "" );
	m_pPingLabel = new Label( this, "PingLabel", "" );
	m_pServerTypeLabel = new Label( this, "ServerTypeLabel", "" );
	m_pRegionLabel = new Label( this, "RegionLabel", "" );
	m_pDescription = new TextEntry( this, "Description" );
	m_pAddonsList = new ListPanel( this, "AddonsList" );
	m_pPlayerList = new ListPanel( this, "PlayerList" );
	m_pConnectButton = new Button( this, "ConnectButton", "#ServerBrowser_Connect" );
	m_pCloseButton = new Button( this, "CloseButton", "#vgui_close" );
	m_pFavoriteButton = new ToggleButton( this, "FavoriteButton", "Favorite" );
	m_pBlacklistButton = new ToggleButton( this, "BlacklistButton", "Blacklist" );
	m_pMoreInfoButton = new Button( this, "MoreInfoButton", "MORE INFO" );
	m_pWarningIcon = new ImagePanel( this, "WarningIcon" );
	m_pLanguageFlag = new ImagePanel( this, "LanguageFlag" );
	m_pRegionFlag = new ImagePanel( this, "RegionFlag" );
	
	m_pConnectButton->SetCommand( "Connect" );
	m_pCloseButton->SetCommand( "Close" );
	
	m_pDescription->SetMultiline( true );
	m_pDescription->SetEditable( false );
	m_pDescription->SetVerticalScrollbar( true );
	
	// Setup player list columns
	m_pPlayerList->AddColumnHeader( 0, "name", "#ServerBrowser_PlayerName", 200 );
	m_pPlayerList->AddColumnHeader( 1, "score", "#ServerBrowser_Score", 60 );
	m_pPlayerList->AddColumnHeader( 2, "time", "#ServerBrowser_Time", 80 );
	
	// Setup addons list columns
	m_pAddonsList->AddColumnHeader( 0, "addon", "Addon", 230 );
	
	m_unServerIP = 0;
	m_usServerPort = 0;
	m_hServerQuery = HSERVERQUERY_INVALID;
	m_hPlayerQuery = HSERVERQUERY_INVALID;
	m_hRulesQuery = HSERVERQUERY_INVALID;
	
	LoadControlSettings( "Resource/UI/ServerBrowser/DialogGameInfo.res" );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CDialogGameInfo::~CDialogGameInfo()
{
	// Cancel any pending queries
	if ( steamapicontext && steamapicontext->SteamMatchmakingServers() )
	{
		if ( m_hServerQuery != HSERVERQUERY_INVALID )
		{
			steamapicontext->SteamMatchmakingServers()->CancelServerQuery( m_hServerQuery );
		}
		if ( m_hPlayerQuery != HSERVERQUERY_INVALID )
		{
			steamapicontext->SteamMatchmakingServers()->CancelServerQuery( m_hPlayerQuery );
		}
		if ( m_hRulesQuery != HSERVERQUERY_INVALID )
		{
			steamapicontext->SteamMatchmakingServers()->CancelServerQuery( m_hRulesQuery );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Apply scheme settings
//-----------------------------------------------------------------------------
void CDialogGameInfo::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	
	SetBgColor( pScheme->GetColor( "ServerBrowser.Background", Color( 32, 32, 32, 255 ) ) );
}

//-----------------------------------------------------------------------------
// Purpose: Layout
//-----------------------------------------------------------------------------
void CDialogGameInfo::PerformLayout()
{
	BaseClass::PerformLayout();
	
	SetSize( 1200, 700 );
	MoveToCenterOfScreen();
}

//-----------------------------------------------------------------------------
// Purpose: Handle commands
//-----------------------------------------------------------------------------
void CDialogGameInfo::OnCommand( const char *command )
{
	if ( !Q_stricmp( command, "Connect" ) )
	{
		char cmd[256];
		Q_snprintf( cmd, sizeof( cmd ), "connect %d.%d.%d.%d:%d",
			( m_unServerIP >> 24 ) & 0xFF,
			( m_unServerIP >> 16 ) & 0xFF,
			( m_unServerIP >> 8 ) & 0xFF,
			m_unServerIP & 0xFF,
			m_usServerPort );
		
		engine->ClientCmd_Unrestricted( cmd );
		OnCommand( "Close" );
	}
	else if ( !Q_stricmp( command, "Close" ) )
	{
		SetVisible( false );
		MarkForDeletion();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set server info
//-----------------------------------------------------------------------------
void CDialogGameInfo::SetServerInfo( gameserveritem_t *pServer )
{
	if ( !pServer )
		return;
	
	V_memcpy( &m_ServerInfo, pServer, sizeof( gameserveritem_t ) );
	
	m_pServerName->SetText( pServer->GetName() );
	m_pMapName->SetText( pServer->m_szMap );
	m_pGameTags->SetText( pServer->m_szGameTags );
	
	char szPlayers[64];
	Q_snprintf( szPlayers, sizeof( szPlayers ), "%d/%d Players", pServer->m_nPlayers, pServer->m_nMaxPlayers );
	m_pPlayerCountLabel->SetText( szPlayers );
	
	char szPing[32];
	Q_snprintf( szPing, sizeof( szPing ), "%d ms", pServer->m_nPing );
	m_pPingLabel->SetText( szPing );
	
	// Determine server type from steam ID
	bool bDedicated = ( pServer->m_steamID.BIndividualAccount() == false );
	m_pServerTypeLabel->SetText( bDedicated ? "Server Type: Dedicated" : "Server Type: Listen" );
	
	m_pDescription->SetText( pServer->m_szGameDescription );
}

//-----------------------------------------------------------------------------
// Purpose: Update server info
//-----------------------------------------------------------------------------
void CDialogGameInfo::UpdateServerInfo( uint32 serverIP, uint16 serverPort )
{
	m_unServerIP = serverIP;
	m_usServerPort = serverPort;
	
	QueryServerInfo();
}

//-----------------------------------------------------------------------------
// Purpose: Query server info
//-----------------------------------------------------------------------------
void CDialogGameInfo::QueryServerInfo()
{
	if ( !steamapicontext || !steamapicontext->SteamMatchmakingServers() )
		return;
	
	// Note: Full implementation would need proper callbacks
	// This is a simplified version
}

//-----------------------------------------------------------------------------
// Purpose: Blacklisted Servers Page
//-----------------------------------------------------------------------------
CBlacklistedServersPage::CBlacklistedServersPage( Panel *parent ) : BaseClass( parent, "BlacklistedServers" )
{
	m_pGameList = new ListPanel( this, "GameList" );
	m_pRemoveButton = new Button( this, "RemoveButton", "#ServerBrowser_RemoveFromBlacklist" );
	m_pRefreshButton = new Button( this, "RefreshButton", "#ServerBrowser_Refresh" );
	
	m_pGameList->AddColumnHeader( 0, "name", "#ServerBrowser_Servers", 400 );
	m_pGameList->AddColumnHeader( 1, "address", "#ServerBrowser_IPAddress", 150 );
	
	m_pRemoveButton->SetCommand( "RemoveFromBlacklist" );
	m_pRemoveButton->SetEnabled( false );
	m_pRefreshButton->SetCommand( "GetNewList" );
	
	LoadControlSettings( "Resource/UI/ServerBrowser/BlacklistedServers.res" );
}

CBlacklistedServersPage::~CBlacklistedServersPage()
{
}

void CBlacklistedServersPage::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	SetBgColor( pScheme->GetColor( "ServerBrowser.Panel", Color( 40, 40, 40, 255 ) ) );
}

void CBlacklistedServersPage::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CBlacklistedServersPage::OnCommand( const char *command )
{
	if ( !Q_stricmp( command, "RemoveFromBlacklist" ) )
	{
		RemoveFromBlacklist();
	}
	else if ( !Q_stricmp( command, "GetNewList" ) )
	{
		RefreshServerList();
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

void CBlacklistedServersPage::RefreshServerList()
{
	m_pGameList->RemoveAll();
	// TODO: Load blacklisted servers from file/config
}

void CBlacklistedServersPage::RemoveFromBlacklist()
{
	int itemID = m_pGameList->GetSelectedItem( 0 );
	if ( itemID >= 0 )
	{
		m_pGameList->RemoveItem( itemID );
		// TODO: Save to file/config
	}
}

//-----------------------------------------------------------------------------
// Purpose: Console command to open server browser
//-----------------------------------------------------------------------------
CON_COMMAND( openserverbrowser, "Opens the Custom Fortress Server Browser" )
{
	CCFServerBrowser::ShowDialog();
}

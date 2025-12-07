"Resource/UI/WorkshopUpload.res"
{
	"WorkshopUpload"
	{
		"ControlName"		"Frame"
		"fieldName"			"WorkshopUpload"
		"xpos"				"c-450"
		"ypos"				"20"
		"wide"				"900"
		"tall"				"760"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"settitlebarvisible"	"0"
		"PaintBackgroundType"	"0"
		"bgcolor_override"		"46 43 42 255"
		"border"				"GrayDialogBorder"
	}
	
	"BackgroundPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"BackgroundPanel"
		"xpos"			"0"
		"ypos"			"0"
		"zpos"			"-1"
		"wide"			"900"
		"tall"			"760"
		"visible"		"1"
		"enabled"		"1"
		"PaintBackgroundType"	"0"
		"bgcolor_override"		"46 43 42 255"
	}
	
	"TitleLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"TitleLabel"
		"xpos"			"20"
		"ypos"			"10"
		"zpos"			"1"
		"wide"			"400"
		"tall"			"30"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"UPLOAD TO WORKSHOP"
		"font"			"HudFontMediumBold"
		"textAlignment"	"west"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"CloseButton"
	{
		"ControlName"	"CExButton"
		"fieldName"		"CloseButton"
		"xpos"			"860"
		"ypos"			"8"
		"zpos"			"10"
		"wide"			"30"
		"tall"			"26"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"X"
		"font"			"HudFontSmallBold"
		"textAlignment"	"center"
		"Command"		"cancel"
		"default"		"0"
		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"
		"border_default"	"TFFatLineBorderOpaque"
		"border_armed"		"TFFatLineBorderRedBGOpaque"
	}
	
	// Left Column - Basic Info
	"ItemTitleLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"ItemTitleLabel"
		"xpos"			"20"
		"ypos"			"50"
		"wide"			"80"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"east"
		"fgcolor_override"	"200 187 161 255"
		"labelText"		"Title:"
	}
	
	"TitleEntry"
	{
		"ControlName"	"TextEntry"
		"fieldName"		"TitleEntry"
		"xpos"			"110"
		"ypos"			"50"
		"wide"			"280"
		"tall"			"24"
		"visible"		"1"
		"enabled"		"1"
		"bgcolor_override"	"30 28 26 255"
		"fgcolor_override"	"200 187 161 255"
		"border"		"TFFatLineBorder"
	}
	
	"VisibilityLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"VisibilityLabel"
		"xpos"			"20"
		"ypos"			"82"
		"wide"			"80"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"east"
		"fgcolor_override"	"200 187 161 255"
		"labelText"		"Visibility:"
	}
	
	"VisibilityCombo"
	{
		"ControlName"	"ComboBox"
		"fieldName"		"VisibilityCombo"
		"xpos"			"110"
		"ypos"			"82"
		"wide"			"280"
		"tall"			"24"
		"visible"		"1"
		"enabled"		"1"
	}
	
	"ContentLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"ContentLabel"
		"xpos"			"20"
		"ypos"			"114"
		"wide"			"80"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"east"
		"fgcolor_override"	"200 187 161 255"
		"labelText"		"Content:"
	}
	
	"ContentPathEntry"
	{
		"ControlName"	"TextEntry"
		"fieldName"		"ContentPathEntry"
		"xpos"			"110"
		"ypos"			"114"
		"wide"			"240"
		"tall"			"24"
		"visible"		"1"
		"enabled"		"1"
		"bgcolor_override"	"30 28 26 255"
		"fgcolor_override"	"200 187 161 255"
		"border"		"TFFatLineBorder"
	}
	
	"BrowseContentButton"
	{
		"ControlName"	"CExButton"
		"fieldName"		"BrowseContentButton"
		"xpos"			"355"
		"ypos"			"114"
		"wide"			"35"
		"tall"			"24"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"..."
		"font"			"HudFontSmallBold"
		"textAlignment"	"center"
		"Command"		"browsecontent"
		"default"		"0"
		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"
		"border_default"	"TFFatLineBorderOpaque"
		"border_armed"		"TFFatLineBorder"
	}
	
	"PreviewLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"PreviewLabel"
		"xpos"			"20"
		"ypos"			"146"
		"wide"			"80"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"east"
		"fgcolor_override"	"200 187 161 255"
		"labelText"		"Preview:"
	}
	
	"PreviewImageEntry"
	{
		"ControlName"	"TextEntry"
		"fieldName"		"PreviewImageEntry"
		"xpos"			"110"
		"ypos"			"146"
		"wide"			"240"
		"tall"			"24"
		"visible"		"1"
		"enabled"		"1"
		"bgcolor_override"	"30 28 26 255"
		"fgcolor_override"	"200 187 161 255"
		"border"		"TFFatLineBorder"
	}
	
	"BrowsePreviewButton"
	{
		"ControlName"	"CExButton"
		"fieldName"		"BrowsePreviewButton"
		"xpos"			"355"
		"ypos"			"146"
		"wide"			"35"
		"tall"			"24"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"..."
		"font"			"HudFontSmallBold"
		"textAlignment"	"center"
		"Command"		"browsepreview"
		"default"		"0"
		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"
		"border_default"	"TFFatLineBorderOpaque"
		"border_armed"		"TFFatLineBorder"
	}
	
	"DescLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"DescLabel"
		"xpos"			"20"
		"ypos"			"178"
		"wide"			"80"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"east"
		"fgcolor_override"	"200 187 161 255"
		"labelText"		"Description:"
	}
	
	"DescriptionEntry"
	{
		"ControlName"	"TextEntry"
		"fieldName"		"DescriptionEntry"
		"xpos"			"110"
		"ypos"			"178"
		"wide"			"280"
		"tall"			"120"
		"visible"		"1"
		"enabled"		"1"
		"bgcolor_override"	"30 28 26 255"
		"fgcolor_override"	"200 187 161 255"
		"border"		"TFFatLineBorder"
		"wrap"			"1"
		"multiline"		"1"
	}
	
	"PreviewImageLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"PreviewImageLabel"
		"xpos"			"20"
		"ypos"			"306"
		"wide"			"80"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"east"
		"fgcolor_override"	"200 187 161 255"
		"labelText"		"Preview:"
	}
	
	"PreviewImagePanel"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"PreviewImagePanel"
		"xpos"			"110"
		"ypos"			"306"
		"wide"			"280"
		"tall"			"210"
		"visible"		"1"
		"enabled"		"1"
		"scaleImage"	"1"
		"bgcolor_override"	"30 28 26 255"
		"border"		"TFFatLineBorder"
	}
	
	// Right Column - Tags
	"TagsHeaderLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"TagsHeaderLabel"
		"xpos"			"410"
		"ypos"			"50"
		"wide"			"470"
		"tall"			"26"
		"font"			"HudFontSmallBold"
		"textAlignment"	"west"
		"fgcolor_override"	"200 187 161 255"
		"labelText"		"TAGS (select all that apply):"
	}
	
	"TagsScrollPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"TagsScrollPanel"
		"xpos"			"410"
		"ypos"			"80"
		"wide"			"470"
		"tall"			"580"
		"visible"		"1"
		"enabled"		"1"
		"bgcolor_override"	"30 28 26 255"
		"border"		"TFFatLineBorder"
	}
	
	// MOD TYPE Category
	"ModTypeCategoryLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"ModTypeCategoryLabel"
		"xpos"			"420"
		"ypos"			"88"
		"wide"			"350"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"west"
		"fgcolor_override"	"235 226 202 255"
		"labelText"		"MOD TYPE:"
	}
	
	"TagCheck_Map"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Map"
		"xpos"			"420"
		"ypos"			"112"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Map"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Skin"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Skin"
		"xpos"			"520"
		"ypos"			"112"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Skin"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_GUIs"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_GUIs"
		"xpos"			"620"
		"ypos"			"112"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"GUIs"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Particles"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Particles"
		"xpos"			"720"
		"ypos"			"112"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Particles"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Sounds"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Sounds"
		"xpos"			"420"
		"ypos"			"136"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Sounds"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Sprays"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Sprays"
		"xpos"			"520"
		"ypos"			"136"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Sprays"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Misc"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Misc"
		"xpos"			"620"
		"ypos"			"136"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Misc."
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	// GAME MODE Category
	"GameModeCategoryLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"GameModeCategoryLabel"
		"xpos"			"420"
		"ypos"			"168"
		"wide"			"350"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"west"
		"fgcolor_override"	"235 226 202 255"
		"labelText"		"GAME MODE (for maps):"
	}
	
	"TagCheck_CTF"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_CTF"
		"xpos"			"420"
		"ypos"			"192"
		"wide"			"140"
		"tall"			"24"
		"labelText"		"Capture The Flag"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_CP"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_CP"
		"xpos"			"560"
		"ypos"			"192"
		"wide"			"130"
		"tall"			"24"
		"labelText"		"Control Point"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Payload"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Payload"
		"xpos"			"700"
		"ypos"			"192"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"Payload"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_PLR"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_PLR"
		"xpos"			"420"
		"ypos"			"216"
		"wide"			"140"
		"tall"			"24"
		"labelText"		"Payload Race"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Arena"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Arena"
		"xpos"			"560"
		"ypos"			"216"
		"wide"			"130"
		"tall"			"24"
		"labelText"		"Arena"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_KOTH"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_KOTH"
		"xpos"			"700"
		"ypos"			"216"
		"wide"			"130"
		"tall"			"24"
		"labelText"		"King of the Hill"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_AD"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_AD"
		"xpos"			"420"
		"ypos"			"240"
		"wide"			"140"
		"tall"			"24"
		"labelText"		"Attack / Defense"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_SD"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_SD"
		"xpos"			"560"
		"ypos"			"240"
		"wide"			"130"
		"tall"			"24"
		"labelText"		"Special Delivery"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_RD"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_RD"
		"xpos"			"700"
		"ypos"			"240"
		"wide"			"140"
		"tall"			"24"
		"labelText"		"Robot Destruction"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_MVM"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_MVM"
		"xpos"			"420"
		"ypos"			"264"
		"wide"			"140"
		"tall"			"24"
		"labelText"		"Mann Vs. Machine"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_MVMVersus"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_MVMVersus"
		"xpos"			"560"
		"ypos"			"264"
		"wide"			"140"
		"tall"			"24"
		"labelText"		"MVM Versus"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Mannpower"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Mannpower"
		"xpos"			"700"
		"ypos"			"264"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"Mannpower"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Medieval"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Medieval"
		"xpos"			"420"
		"ypos"			"288"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"Medieval"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_PASS"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_PASS"
		"xpos"			"530"
		"ypos"			"288"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"PASS Time"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_PD"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_PD"
		"xpos"			"640"
		"ypos"			"288"
		"wide"			"150"
		"tall"			"24"
		"labelText"		"Player Destruction"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Custom"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Custom"
		"xpos"			"420"
		"ypos"			"312"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Custom"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	// CLASS Category
	"ClassCategoryLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"ClassCategoryLabel"
		"xpos"			"420"
		"ypos"			"344"
		"wide"			"350"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"west"
		"fgcolor_override"	"235 226 202 255"
		"labelText"		"CLASS (for items/skins):"
	}
	
	"TagCheck_Scout"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Scout"
		"xpos"			"420"
		"ypos"			"368"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Scout"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Soldier"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Soldier"
		"xpos"			"520"
		"ypos"			"368"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Soldier"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Pyro"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Pyro"
		"xpos"			"620"
		"ypos"			"368"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Pyro"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Demoman"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Demoman"
		"xpos"			"420"
		"ypos"			"392"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Demoman"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Heavy"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Heavy"
		"xpos"			"520"
		"ypos"			"392"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Heavy"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Engineer"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Engineer"
		"xpos"			"620"
		"ypos"			"392"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Engineer"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Medic"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Medic"
		"xpos"			"420"
		"ypos"			"416"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Medic"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Sniper"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Sniper"
		"xpos"			"520"
		"ypos"			"416"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Sniper"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Spy"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Spy"
		"xpos"			"620"
		"ypos"			"416"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Spy"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	// ITEM SLOT Category
	"ItemSlotCategoryLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"ItemSlotCategoryLabel"
		"xpos"			"420"
		"ypos"			"448"
		"wide"			"350"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"west"
		"fgcolor_override"	"235 226 202 255"
		"labelText"		"ITEM SLOT:"
	}
	
	"TagCheck_Primary"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Primary"
		"xpos"			"420"
		"ypos"			"472"
		"wide"			"150"
		"tall"			"24"
		"labelText"		"Weapon (Primary)"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Secondary"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Secondary"
		"xpos"			"570"
		"ypos"			"472"
		"wide"			"170"
		"tall"			"24"
		"labelText"		"Weapon (Secondary)"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Melee"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Melee"
		"xpos"			"420"
		"ypos"			"496"
		"wide"			"150"
		"tall"			"24"
		"labelText"		"Weapon (Melee)"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_PDA"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_PDA"
		"xpos"			"570"
		"ypos"			"496"
		"wide"			"150"
		"tall"			"24"
		"labelText"		"Weapon (PDA)"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Cosmetic"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Cosmetic"
		"xpos"			"420"
		"ypos"			"520"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"Cosmetic"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Action"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Action"
		"xpos"			"530"
		"ypos"			"520"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Action"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Taunt"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Taunt"
		"xpos"			"630"
		"ypos"			"520"
		"wide"			"100"
		"tall"			"24"
		"labelText"		"Taunt"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_SlotSkin"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_SlotSkin"
		"xpos"			"730"
		"ypos"			"520"
		"wide"			"80"
		"tall"			"24"
		"labelText"		"Skin"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	// OTHER Category
	"OtherCategoryLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"OtherCategoryLabel"
		"xpos"			"420"
		"ypos"			"552"
		"wide"			"350"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"west"
		"fgcolor_override"	"235 226 202 255"
		"labelText"		"OTHER:"
	}
	
	"TagCheck_Halloween"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Halloween"
		"xpos"			"420"
		"ypos"			"576"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"Halloween"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Christmas"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Christmas"
		"xpos"			"530"
		"ypos"			"576"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"Christmas"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Summer"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Summer"
		"xpos"			"640"
		"ypos"			"576"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"Summer"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Unusual"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Unusual"
		"xpos"			"420"
		"ypos"			"600"
		"wide"			"130"
		"tall"			"24"
		"labelText"		"Unusual Effect"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_WarPaint"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_WarPaint"
		"xpos"			"530"
		"ypos"			"600"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"War Paint"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Killstreak"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Killstreak"
		"xpos"			"640"
		"ypos"			"600"
		"wide"			"130"
		"tall"			"24"
		"labelText"		"Killstreak Effect"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_CommunityFix"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_CommunityFix"
		"xpos"			"420"
		"ypos"			"625"
		"wide"			"130"
		"tall"			"24"
		"labelText"		"Community Fix"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	// Status and Progress
	"StatusLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"StatusLabel"
		"xpos"			"20"
		"ypos"			"670"
		"wide"			"370"
		"tall"			"24"
		"font"			"HudFontSmall"
		"textAlignment"	"west"
		"fgcolor_override"	"200 187 161 255"
		"labelText"		""
	}
	
	"ProgressBar"
	{
		"ControlName"	"ProgressBar"
		"fieldName"		"ProgressBar"
		"xpos"			"20"
		"ypos"			"695"
		"wide"			"370"
		"tall"			"24"
		"visible"		"0"
		"enabled"		"1"
	}
	
	// Buttons
	"UploadButton"
	{
		"ControlName"	"CExButton"
		"fieldName"		"UploadButton"
		"xpos"			"670"
		"ypos"			"720"
		"wide"			"100"
		"tall"			"30"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"Upload"
		"font"			"HudFontSmallBold"
		"textAlignment"	"center"
		"Command"		"upload"
		"default"		"1"
		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"
		"border_default"	"TFFatLineBorderOpaque"
		"border_armed"		"TFFatLineBorderRedBG"
	}
	
	"CancelButton"
	{
		"ControlName"	"CExButton"
		"fieldName"		"CancelButton"
		"xpos"			"780"
		"ypos"			"720"
		"wide"			"100"
		"tall"			"30"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"Cancel"
		"font"			"HudFontSmallBold"
		"textAlignment"	"center"
		"Command"		"cancel"
		"default"		"0"
		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"
		"border_default"	"TFFatLineBorderOpaque"
		"border_armed"		"TFFatLineBorder"
	}
}

"Resource/UI/WorkshopUpdate.res"
{
	"WorkshopUpdate"
	{
		"ControlName"		"Frame"
		"fieldName"			"WorkshopUpdate"
		"xpos"				"c-300"
		"ypos"				"c-275"
		"wide"				"600"
		"tall"				"550"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"tabPosition"		"0"
		"settitlebarvisible"	"1"
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
		"wide"			"600"
		"tall"			"550"
		"visible"		"1"
		"enabled"		"1"
		"PaintBackgroundType"	"0"
		"bgcolor_override"		"46 43 42 255"
	}
	
	"HeaderLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"HeaderLabel"
		"xpos"			"20"
		"ypos"			"10"
		"zpos"			"1"
		"wide"			"400"
		"tall"			"30"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"UPDATE ITEM METADATA"
		"font"			"HudFontMediumBold"
		"textAlignment"	"west"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"InfoLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"InfoLabel"
		"xpos"			"20"
		"ypos"			"40"
		"wide"			"560"
		"tall"			"20"
		"font"			"HudFontSmall"
		"textAlignment"	"west"
		"fgcolor_override"	"150 140 120 255"
		"labelText"		"Edit the title, description, and visibility. Content and preview are unchanged."
	}
	
	// Title
	"ItemTitleLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"ItemTitleLabel"
		"xpos"			"20"
		"ypos"			"70"
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
		"ypos"			"70"
		"wide"			"470"
		"tall"			"24"
		"visible"		"1"
		"enabled"		"1"
		"bgcolor_override"	"30 28 26 255"
		"fgcolor_override"	"200 187 161 255"
		"border"		"TFFatLineBorder"
	}
	
	// Visibility
	"VisibilityLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"VisibilityLabel"
		"xpos"			"20"
		"ypos"			"102"
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
		"ypos"			"102"
		"wide"			"200"
		"tall"			"24"
		"visible"		"1"
		"enabled"		"1"
	}
	
	// Description
	"DescLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"DescLabel"
		"xpos"			"20"
		"ypos"			"134"
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
		"ypos"			"134"
		"wide"			"470"
		"tall"			"150"
		"visible"		"1"
		"enabled"		"1"
		"bgcolor_override"	"30 28 26 255"
		"fgcolor_override"	"200 187 161 255"
		"border"		"TFFatLineBorder"
		"wrap"			"1"
		"multiline"		"1"
	}
	
	// Tags Section
	"TagsHeaderLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"TagsHeaderLabel"
		"xpos"			"20"
		"ypos"			"295"
		"wide"			"560"
		"tall"			"24"
		"font"			"HudFontSmallBold"
		"textAlignment"	"west"
		"fgcolor_override"	"200 187 161 255"
		"labelText"		"TAGS:"
	}
	
	"TagCheck_Map"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Map"
		"xpos"			"20"
		"ypos"			"320"
		"wide"			"90"
		"tall"			"24"
		"labelText"		"Map"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Skin"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Skin"
		"xpos"			"110"
		"ypos"			"320"
		"wide"			"90"
		"tall"			"24"
		"labelText"		"Skin"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Weapon"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Weapon"
		"xpos"			"200"
		"ypos"			"320"
		"wide"			"110"
		"tall"			"24"
		"labelText"		"Weapon Mod"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_GUIs"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_GUIs"
		"xpos"			"310"
		"ypos"			"320"
		"wide"			"90"
		"tall"			"24"
		"labelText"		"GUIs"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Particles"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Particles"
		"xpos"			"400"
		"ypos"			"320"
		"wide"			"90"
		"tall"			"24"
		"labelText"		"Particles"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Sounds"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Sounds"
		"xpos"			"490"
		"ypos"			"320"
		"wide"			"90"
		"tall"			"24"
		"labelText"		"Sounds"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Sprays"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Sprays"
		"xpos"			"20"
		"ypos"			"344"
		"wide"			"90"
		"tall"			"24"
		"labelText"		"Sprays"
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	"TagCheck_Misc"
	{
		"ControlName"	"CheckButton"
		"fieldName"		"TagCheck_Misc"
		"xpos"			"110"
		"ypos"			"344"
		"wide"			"90"
		"tall"			"24"
		"labelText"		"Misc."
		"font"			"HudFontSmall"
		"fgcolor_override"	"200 187 161 255"
	}
	
	// Status and Progress
	"StatusLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"StatusLabel"
		"xpos"			"20"
		"ypos"			"460"
		"wide"			"560"
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
		"ypos"			"485"
		"wide"			"560"
		"tall"			"16"
		"visible"		"0"
		"enabled"		"1"
	}
	
	// Buttons
	"UpdateButton"
	{
		"ControlName"	"CExButton"
		"fieldName"		"UpdateButton"
		"xpos"			"370"
		"ypos"			"510"
		"wide"			"100"
		"tall"			"30"
		"visible"		"1"
		"enabled"		"1"
		"labelText"		"Update"
		"font"			"HudFontSmallBold"
		"textAlignment"	"center"
		"Command"		"update"
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
		"xpos"			"480"
		"ypos"			"510"
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

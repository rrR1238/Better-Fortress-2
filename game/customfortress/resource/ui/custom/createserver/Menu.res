"Resource\UI\TFModServerDialog_TF2UI.res"
{
	"TFModServerDialog"
	{
		"ControlName"		"EditablePanel"
		"fieldName"		"TFModServerDialog"
		"xpos"		"c-250"
		"ypos"		"90"
		"wide"		"500"
		"tall"		"400"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"infocus_bgcolor_override"				"50 50 50 255"
		"outoffocus_bgcolor_override"			"50 50 50 255"
		"paintbackgroundtype"	"2"
		"settitlebarvisible"	"0"
		"paintborder"			"1"
		"paintbackground"		"0"
		"border"				"GrayDialogBorder"
		
		"control_w"			"500"
		"control_h"			"25"
		"slider_w"			"500"
		"slider_h"			"25"
		
		"clientinsetx_override"			"0"
		"sheetinset_bottom"				"40"
		
	}
	
	
	"Sheet"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"Sheet"
		"tabxindent"	"2"
		"tabxdelta"		"10"
		"tabwidth"		"240"
		"tabheight"		"20"
		"transition_time" "0"
		"yoffset"	"6"
		
		"HeaderLine"
		{
			"ControlName"	"ImagePanel"
			"fieldName"		"HeaderLine"
			"xpos"			"0"
			"ypos"			"32"
			"zpos"			"5"
			"wide"			"f0"
			"tall"			"10"
			"visible"		"1"
			"enabled"		"1"
			"image"			"loadout_solid_line"
			"scaleImage"	"1"
		}				
		
		// No idea why but i have to do this instead of using tabskv
		"tab"
		{
			"fieldName"			"tab_styled"
			"textinsetx"		"40"
			"font"				"HudFontMediumSmallBold"
			"selectedcolor"		"200 187 161 255"
			"unselectedcolor"	"130 120 104 255"	
			"defaultBgColor_override"	"46 43 42 255"
			"paintbackground"	"0"
			"activeborder_override"	"OutlinedGreyBox"
			"normalborder_override" "OutlinedDullGreyBox"
		}
		"tab"
		{
			"fieldName"			"tab_styled"
			"textinsetx"		"40"
			"font"				"HudFontMediumSmallBold"
			"selectedcolor"		"200 187 161 255"
			"unselectedcolor"	"130 120 104 255"	
			"defaultBgColor_override"	"46 43 42 255"
			"paintbackground"	"0"
			"activeborder_override"	"OutlinedGreyBox"
			"normalborder_override" "OutlinedDullGreyBox"
		}
		"tab"
		{
			"fieldName"			"tab_styled"
			"textinsetx"		"40"
			"font"				"HudFontMediumSmallBold"
			"selectedcolor"		"200 187 161 255"
			"unselectedcolor"	"130 120 104 255"	
			"defaultBgColor_override"	"46 43 42 255"
			"paintbackground"	"0"
			"activeborder_override"	"OutlinedGreyBox"
			"normalborder_override" "OutlinedDullGreyBox"
		}
	}
	
	"CloseButton"
	{
		"ControlName"	"CExButton"
		"fieldName"		"CloseButton"
		"xpos"			"470"
		"ypos"			"5"
		"zpos"			"1"
		"wide"			"25"
		"tall"			"25"
		"autoResize"	"0"
		"pinCorner"		"2"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"
		"labelText"		"X"
		"font"			"HudFontSmallBold"
		"textAlignment"	"center"
		"dulltext"		"0"
		"brighttext"	"0"
		"default"		"1"
		"Command"		"Close"
		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"
	}
	
	"#GameUI_Server"
	{
		"ControlName"		"CPanelListPanel"
		"fieldName"		"#GameUI_Server"
		"xpos"		"10"
		"ypos"		"40"
		"wide"		"480"
		"tall"		"260"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"bgcolor_override"	"0 0 0 0"
		
		
		// This is incredibly stupid
		"PanelListEmbedded"
		{
			"ControlName"		"EditablePanel"
			"fieldName"			"PageOneEmbedded"
			
			"map_preview"
			{
				"tall"			"45"
				"border"		"none"
			}	
			
			"map_preview_border"
			{
				"ControlName"	"EditablePanel"
				"fieldName"		"map_preview_border"
				"border"		"MainMenuBGBorder"
				
				"xpos"			"4"
				"ypos"			"220"
				"wide"			"74"
				"tall"			"60"
			}	
			"map_preview_img"
			{
				"ControlName"	"ImagePanel"
				"fieldName"		"map_preview_img"
				"visible"		"1"
				"enabled"		"1"
				"image"			"maps/menu_thumb_default"
				
				"xpos"			"12"
				"ypos"			"228"
				"wide"			"64"
				"tall"			"43"
			}
		}
	}
	
	"#Replay_Contest_Rules"
	{
		"ControlName"		"CPanelListPanel"
		"fieldName"		"#Replay_Contest_Rules"
		"xpos"		"10"
		"ypos"		"40"
		"wide"		"480"
		"tall"		"260"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
		"tabPosition"		"0"
		"bgcolor_override"	"0 0 0 0"
	}
	
	"#GameUI_GameMenu_Options"
	{
		"ControlName"		"CPanelListPanel"
		"fieldName"		"#GameUI_GameMenu_Options"
		"xpos"		"10"
		"ypos"		"40"
		"wide"		"480"
		"tall"		"260"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
		"tabPosition"		"0"
		"bgcolor_override"	"0 0 0 0"
	}
	
	"CreateServerButton"
	{
		"ControlName"	"CExButton"
		"fieldName"		"CreateServerButton"
		"xpos"			"350"
		"ypos"			"360"
		"zpos"			"1"
		"wide"			"100"
		"tall"			"25"
		"autoResize"	"0"
		"pinCorner"		"3"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"	"0"
		"labelText"		"#GameUI_OK"
		"font"			"HudFontSmallBold"
		"textAlignment"	"center"
		"dulltext"		"0"
		"brighttext"	"0"
		"default"		"1"
		"Command"		"CreateServer"
		"sound_depressed"	"UI/buttonclick.wav"
		"sound_released"	"UI/buttonclickrelease.wav"
	}
	
	"TooltipPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"TooltipPanel"
		"xpos"			"0"
		"ypos"			"0"
		"zpos"			"100"
		"wide"			"240"
		"tall"			"50"
		"visible"		"0"
		"PaintBackgroundType"	"2"
		"border"		"MainMenuBGBorder"
		
		"TipLabel"
		{
			"ControlName"	"CExLabel"
			"fieldName"		"TipLabel"
			"font"			"HudFontSmallest"
			"labelText"		"%tiptext%"
			"textAlignment"	"center"
			"xpos"			"20"
			"ypos"			"10"
			"zpos"			"2"
			"wide"			"200"
			"tall"			"30"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor_override"	"235 226 202 255"
			"wrap"			"1"
			//"centerwrap"	"1"
		}
	}
}

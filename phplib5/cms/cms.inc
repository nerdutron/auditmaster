<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class cms
{
	function _get($Content)
	{
		$Object = 0;
		$Data = DBRow(CMS_DB, "SELECT * FROM content_master WHERE content_id = ".($Content * 1));
		if (sizeof($Data))
		{
			if ($Data["class_file"])
				require_once(INCLUDE_DIR.$Data["class_file"]);
			else
				require_once(PHPLIB_DIR."cms/".$Data["data_type"].".inc");

			$Object = new $Data["data_type"];
			$Object->MasterData = $Data;
		}
		return ($Object);
	}

	function logged_in()
	{
		// login override
		if (defined("CMS_LOGIN"))
			return (CMS_LOGIN);

		// determine login
		$User = $_SERVER["REMOTE_USER"];
		if ($User)
		{
			$Query = "SELECT launch FROM content_editors WHERE remote_user = '".addslashes($User)."'";
			$Launch = DBGet(CMS_DB, $Query);
			return ($Launch);
		}
		else
			return (0);
	}

	function header()
	{
		$CMSData = $_REQUEST["CMSData"];
		$Log = cms::logged_in();
		if ($Log && is_array($CMSData))
		{
			$Object = cms::_get($CMSData["content_id"]);
			if ($CMSData["action"] == "Save")
				$Save = $Object->save($CMSData);
			if ($CMSData["action"] == "Launch" && $Log == "Y")
				$Launch = $Object->launch($CMSData);
			if ($CMSData["action"] == "Delete" && $Log == "Y")
				$Delete = $Object->delete($CMSData["content_id"], $CMSData["item_id"]);
			if ($Save || $Launch || $Delete)
			{
				echo "<script>\n";
				echo "window.opener.location.reload();\n";
				echo "window.close();\n";
				echo "</script>\n";
			}
		}
		if ($Log && is_array($_REQUEST["drag_".$_REQUEST["Content"]]))
		{
			$Object = cms::_get($_REQUEST["Content"]);
//			if ($Object->resequence($_REQUEST["Content"], $_REQUEST["drag_".$_REQUEST["Content"]]))
			$Object->resequence($_REQUEST["Content"], $_REQUEST["drag_".$_REQUEST["Content"]]);
				echo "1";
			exit;
		}
	}

	function _mce($Mode)
	{
		$Set1 = "print,preview,separator,bold,italic,underline,separator,cut,copy,paste,separator,search,replace,separator,undo,redo,separator,removeformat,separator,fullscreen,code";
		$Set2 = "formatselect,justifyleft,justifycenter,justifyright,justifyfull,bullist,numlist,outdent,indent,separator,sub,sup,charmap,separator,hr";
		$Set3 = "image,link,unlink,separator,tablecontrols,separator,visualaid";
		if (defined("CMS_ROW1"))
			$Set1 = str_replace("SET", $Set1, CMS_ROW1);
		if (defined("CMS_ROW2"))
			$Set2 = str_replace("SET", $Set2, CMS_ROW2);
		if (defined("CMS_ROW3"))
			$Set3 = str_replace("SET", $Set3, CMS_ROW3);
		if ($Mode)
		{
			echo "<!-- tinyMCE -->\n";
			echo "<script language=\"javascript\" type=\"text/javascript\" src=\"tiny_mce/tiny_mce.js\"></script>\n";
			echo "<script language=\"javascript\" type=\"text/javascript\">\n";

			echo "	tinyMCE.init({\n";
			echo "		theme : \"advanced\",\n";
			echo "		mode : \"exact\",\n";
			if (defined("CMS_CSS"))
				echo "		content_css : \"".CMS_CSS."\",\n";
			if (defined("CMS_LINKS"))
				echo "		external_link_list_url : \"".CMS_LINKS."\",\n";
			if (defined("CMS_IMAGES"))
				echo "		external_image_list_url : \"".CMS_IMAGES."\",\n";
			echo "		elements : \"editor\",\n";
			echo "		theme_advanced_toolbar_location : \"top\",\n";
			echo "		theme_advanced_toolbar_align : \"left\",\n";

			switch ($Mode)
			{
				case 1:
					echo "		plugins : \"preview, searchreplace, print, fullscreen\",\n";
					echo "		theme_advanced_buttons1 : \"".$Set1."\",\n";
					echo "		theme_advanced_buttons2 : \"\",\n";
					echo "		theme_advanced_buttons3 : \"\"\n";
					break;
				case 2:
					echo "		plugins : \"preview, searchreplace, print, fullscreen\",\n";
					echo "		theme_advanced_buttons1 : \"".$Set1."\",\n";
					echo "		theme_advanced_buttons2 : \"".$Set2."\",\n";
					echo "		theme_advanced_buttons3 : \"\"\n";
					break;
				case 3:
//					echo "		plugins : \"preview, searchreplace, print, fullscreen, table, advimage, advlink\",\n";
					echo "		plugins : \"preview, searchreplace, print, fullscreen, table\",\n";
					echo "		theme_advanced_buttons1 : \"".$Set1."\",\n";
					echo "		theme_advanced_buttons2 : \"".$Set2."\",\n";
					echo "		theme_advanced_buttons3 : \"".$Set3."\"\n";
					break;
			}
			echo "	});\n";

			echo "</script>\n";
			echo "<!-- /tinyMCE -->\n";
		}
	}

	function _style()
	{
		echo "<style>\n";
		echo "FIELDSET.cms \n";
		echo "{\n";
		echo "	border: 1px solid #00CC00;\n";
		echo "	margin: 0px;\n";
		echo "	border-collapse: collapse;\n";
		echo "}\n";
		echo "FIELDSET.cms LEGEND\n";
		echo "{\n";
		echo "	font-size: 1.0em;\n";
		echo "	color: #00CC00;\n";
		echo "	font-weight: bold;\n";
		echo "}\n";
		echo "FIELDSET.cms_alert \n";
		echo "{\n";
		echo "	border: 1px solid #CC0000;\n";
		echo "	margin: 0px;\n";
		echo "	border-collapse: collapse;\n";
		echo "}\n";
		echo "FIELDSET.cms_alert LEGEND\n";
		echo "{\n";
		echo "	font-size: 1.0em;\n";
		echo "	color: #CC0000;\n";
		echo "	font-weight: bold;\n";
		echo "}\n";
		echo ".drag_list div\n";
		echo "{\n";
		echo "	cursor: move;\n";
		echo "}\n";
		echo "div.dropmarker\n";
		echo "{\n";
		echo "	height: 5px;\n";
		echo "	width: 25px;\n";
		echo "	border-left: solid 25px #FF0000;\n";
		echo "	background-color: #FF6666;\n";
		echo "	border-right: solid 25px #FFAAAA;\n";
		echo "	margin-top: -3px;\n";
		echo "	margin-left: -25px;\n";
		echo "	z-index: 1000;\n";
		echo "	overflow: hidden;\n";
		echo "}\n";
		echo "</style>\n";
	}


	function _popup()
	{
		$Params = array();
		$Params[] = "toolbar=no";
		$Params[] = "location=no";
		$Params[] = "directories=no";
		$Params[] = "status=no";
		$Params[] = "menubar=no";
		$Params[] = "scrollbars=yes";
		$Params[] = "resizable=yes";
		$Params[] = "copyhistory=yes";

		// edit js popup code
		echo "<script><!--\n";
		echo "function LaunchCMS(EditProg)\n";
		echo "{\n";
		echo "	Rand = Math.round(Math.random()*1000);\n";
		echo "	EditProg = EditProg + '&RN=' + Rand;\n";
		$Popup = $Params;
		$Popup[] = "width=1";
		$Popup[] = "height=1";
		echo "	window.open(EditProg, 'LaunchCMS', '".implode(",", $Popup)."');\n";
		echo "}\n";
		echo "function EditCMS(EditProg)\n";
		echo "{\n";
		echo "	Rand = Math.round(Math.random()*1000);\n";
		echo "	EditProg = EditProg + '&RN=' + Rand;\n";
		$Popup = $Params;
		$Popup[] = "width=750";
		$Popup[] = "height=550";
		echo "	window.open(EditProg, 'EditCMS', '".implode(",", $Popup)."');\n";
		echo "}\n";
		echo "// --></script>\n";
	}

	function simple_get($Content, $Item = 0)
	{
		$Output = "";

		// show edit
		$Log = cms::logged_in();
		$Object = cms::_get($Content);
		if (get_class($Object))
		{
			switch ($Object->mode())
			{
				case "ITEM":
					if ($Log)
						$Record = $Object->get_edit($Content, $Item);
					else
						$Record = $Object->get_www($Content, $Item);
					$Output .= $Record["data"];
					break;
				case "LIST":
				case "SUMMARY":
					if ($Log)
						$Records = $Object->get_edit($Content);
					else
						$Records = $Object->get_www($Content);
					foreach ($Records as $Key => $Record)
						if ($Key !== "edit")
						{
							$Text  = "";
							$Text .= "<h3>".$Record["title"]."</h3>";
							$Text .= "<p>".$Record["data"]."</p>";
							$Output .= cms::display($Text);
						}
					break;
			}
		}
		return ($Output);
	}

	function get($Content, $Item = 0)
	{
		$Output = "";

		// show edit
		$Log = cms::logged_in();
		$Object = cms::_get($Content);
		if (get_class($Object))
		{
			switch ($Object->mode())
			{
				case "ITEM":
					if ($Log)
						$Record = $Object->get_edit($Content, $Item);
					else
						$Record = $Object->get_www($Content, $Item);
					$Output .= cms::display($Object->MasterData["description"], "<p>".$Record["data"]."</p>", $Object->mode(), $Log, $Content, $Record["item_id"], $Record["edit"]);
					break;
				case "LIST":
				case "SUMMARY":
					if ($Object->order() == "DESC" && $Log)
						$Output .= cms::display($Object->MasterData["description"], "<p><i>Add new article.</i></p>", $Object->mode(), $Log, $Content);
					if ($Object->order() == "SEQ")
						$Output .= "<div id=\"drag_".$Content."\">";
					if ($Log)
						$Records = $Object->get_edit($Content);
					else
						$Records = $Object->get_www($Content);
					if ($Object->mode() == "SUMMARY")
					{
						$Output .= "<ul>\n";
						foreach ($Records as $Key => $Record)
						{
							$Output .= "<li>";
							$Output .= "<a href=\"#item_".$Record["item_id"]."\">";
							$Output .= $Record["title"];
							$Output .= "</a>";
							$Output .= "</li>\n";
						}
						$Output .= "</ul>\n";
					}
					foreach ($Records as $Key => $Record)
					{
						if ($Key !== "edit")
						{
							$Text  = "";
							$Text .= "<a name=\"item_".$Record["item_id"]."\"></a>";
							$Text .= "<h3>".$Record["title"]."</h3>";
							if ($Object->order() != "SEQ")
								$Text .= "<h5>Updated: ".date($Object->when(), CreateTimeStamp($Record["target"]))."</h5>";
							$Text .= "<p>".$Record["data"]."</p>";
							if ($Object->order() == "SEQ")
								$Output .= "<div id=\"item_".$Record["item_id"]."\">";
							$Output .= cms::display($Object->MasterData["description"], $Text, $Object->mode(), $Log, $Content, $Record["item_id"], $Records["edit"]);
							if ($Object->order() == "SEQ")
								$Output .= "</div>";
						}
					}
					if ($Object->order() == "SEQ")
					{
						$Output .= "</div>\n";
						$Output .= "<script type=\"text/javascript\" language=\"javascript\">\n";
						$Output .= "Sortable.create('drag_".$Content."',\n";
						$Output .= "	{\n";
						$Output .= "		tag:'div',\n";
//						$Output .= "		ghosting:true,\n";
						$Output .= "		constraint:'vertical',\n";
						$Output .= "		onUpdate:function()\n";
						$Output .= "		{\n";
						$Output .= "			url = 'index.htm?Action=cms_popup&Content=".$Content."&' + Sortable.serialize('drag_".$Content."');\n";
						$Output .= "			new Ajax.Request(url,\n";
						$Output .= "				{\n";
						$Output .= "					onSuccess:function(t)\n";
						$Output .= "					{\n";
						$Output .= "						if (t.responseText)\n";
						$Output .= "							window.location.reload();\n";
						$Output .= "					}\n";
						$Output .= "				}\n";
						$Output .= "			);\n";
						$Output .= "		}\n";
						$Output .= "	}\n";
						$Output .= ")\n";
						$Output .= "</script>\n";
					}
					if ($Object->order() != "DESC" && $Log)
						$Output .= cms::display($Object->MasterData["description"], "<p><i>Add new article.</i></p>", $Object->mode(), $Log, $Content);
					break;
			}
		}
		return ($Output);
	}

	function show($Content, $Item = 0)
	{
		echo cms::get($Content, $Item);
	}

	function display($Description, $Text, $Mode, $Log, $Content, $Item = 0, $Edit = 0)
	{
		$Output = "";
		if ($Log)
		{
			$Link  = "";
			$Link .= "<a href=\"javascript:EditCMS('";
			$Link .= "index.htm?Action=cms_popup&Content=".$Content."&Item=".$Item;
			$Link .= "')\">";
			if ($Item)
				$Link .= "Edit";
			else
				$Link .= "Add";
			$Link .= "</a>";
			$URL  = "";
			$URL .= "index.htm?Action=cms_popup";
			$URL .= "&Content=".$Content;
			$URL .= "&Item=".$Item;
			$URL .= "&CMSData[content_id]=".$Content;
			$URL .= "&CMSData[item_id]=".$Item;
			$URL .= "&CMSData[action]=";

			if ($Edit)
			{
				if ($Log == "Y")
					$Link .= " | <a href=\"javascript:LaunchCMS('".$URL."Launch')\">Launch</a>";
			}
			if ($Item && in_array($Mode, array("LIST", "SUMMARY")))
				$Link .= " | <a href=\"javascript:LaunchCMS('".$URL."Delete')\" onclick=\"return confirm ('Delete this item?')\">Delete</a>";

//			if (defined("CMS_LABEL"))
//				$Link = "<sup>[ ".$Link." ]</sup> ".$Description;

			if ($Edit)
				$Output .= field::GetFieldsetStart($Link, "class=\"cms_alert\"");
			else
				$Output .= field::GetFieldsetStart($Link, "class=\"cms\"");

			if (defined("CMS_LABEL"))
				$Output .= "<div style=\"text-align: right; font-weight: bold; font-size: 0.75em;\">[".$Description."]</div>";
		}
		$Output .= $Text;
		if ($Log)
			$Output .= field::GetFieldsetEnd();
		return ($Output);
	}

	function page_head()
	{
		if (cms::logged_in())
		{
			cms::_style();
			cms::_popup();
		}
	}

	function head()
	{
		$Content = $_REQUEST["Content"];

		if (cms::logged_in())
		{
			$Object = cms::_get($Content);
			$Mode = $Object->mce();
			cms::_mce($Mode);
		}
	}

	function edit()
	{
		$Content = $_REQUEST["Content"];
		$Item = $_REQUEST["Item"];
		if (cms::logged_in())
		{
			$Object = cms::_get($Content);
			$Object->edit($Content, $Item);
		}
	}

//	function body($Content, $Item = "")
//	{
//	}

//	function url($Extras = "")
//	{
//		$Document = $_SERVER["SCRIPT_NAME"]."?Action=".CURRENT_ACTION;
//		foreach ($Extras as $Key => $Val)
//			$URL .= "&".$Key."=".urlencode($Val);
//		return($URL);
//	}
}

?>

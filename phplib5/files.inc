<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."table.inc");
require_once(PHPLIB_DIR."database.inc");

class files
{
	function head()
	{
		// validate that FILE_BROWSER has been defined
		files::ValidateFileBrowser();
		
		if ($_REQUEST["FileUpload"])
		{
			files::save();
			Header("Location: ".files::url());
			exit;
		}
		if ($_REQUEST["FileEXT"])
			files::icon();
		if ($_REQUEST["FileIcon"])
			files::icon();
		if ($_REQUEST["FileThumb"])
			files::thumb();
		if ($_REQUEST["FileShow"])
			files::show();
		if ($_REQUEST["FileDelete"])
			files::delete();
	}

	function body()
	{
		// validate that FILE_BROWSER has been defined
		files::ValidateFileBrowser();

		echo "<script><!--\n";
		echo "function FilePopup(EditProg)\n";
		echo "{\n";
		echo "	Rand = Math.round(Math.random()*1000);\n";
		echo "	EditProg = EditProg + '&RN=' + Rand;\n";
		echo "	window.open(EditProg, 'FilePopup', 'toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=600,height=400');\n";
		echo "}\n";
		echo "// --></script>\n";

		// if there are errors - display..
		if ($_REQUEST["Error"])
			echo "<p class=\"error\">".$_REQUEST["Error"]."</p>\n";

		if ($_REQUEST["FileEdit"])
		{
			$Query = "SELECT * from file_items WHERE file_id = ".$_REQUEST["FileEdit"];
			$FileRecord = DBRow(FILE_DB, $Query);
			$Query = "SELECT mime_desc FROM mime_types WHERE extension = '".$FileRecord["ext"]."'";
			$FileRecord["type"] = DBGet(WEB_DB, $Query);

		}

		$Style = "width: 50px; float: left; text-align: right; font-weight: bold; padding: 2px;";
		field::PutFormStart(files::url(), 1);
		if (is_array($FileRecord))
			field::PutHidden("FileID", $FileRecord["file_id"]);
		else
			field::PutHidden("FileID", 0);
		field::PutHidden("FileUpload", 1);
		echo "<div>";
		echo "<span style=\"".$Style."\">Title:</span>";
		if (is_array($FileRecord))
			field::PutText("FileTitle", $FileRecord["title"], 32, 64);
		else
			field::PutText("FileTitle", "", 32, 64);
		echo "</div>";
		echo "<div>";
		echo "<span style=\"".$Style."\">File:</span>";
		field::PutFile("FileData");
		echo "</div>";
		echo "<div>";
		if (is_array($FileRecord))
		{
			echo "<span style=\"".$Style."\"></span>";
			echo "Currently a <b>";

			$Size = strlen(gzuncompress(base64_decode($FileRecord["data"])));
			if ($Size > 1000000)
				echo number_format(($Size / 1000000), 2)." megabyte ";
			elseif ($Size > 1000)
				echo number_format(($Size / 1000), 2)." kilobyte ";
			else
				echo $Size." byte ";

			echo $FileRecord["type"];
			echo "</b> last updated <b>";
			$Stamp = CreateTimeStamp($FileRecord["updated"]);
			if (date("Y-m-d") == date("Y-m-d", $Stamp))
				echo " Today ";
			elseif (date("Y-m-d", strtotime("1 day ago")) == date("Y-m-d", $Stamp))
				echo " Yesterday ";
			elseif (date("Y-m-d", strtotime("7 days ago")) >= date("Y-m-d", $Stamp))
				echo " last ".date("l", $Stamp)." ";
			else
				echo " last ".date("j/m/y", $Stamp)." ";

			echo " at ".date("g:ia", $Stamp);
			echo "</b>.";
		}
		echo "</div>";
		echo "<div>";
		echo "<span style=\"".$Style."\"></span>";
		field::PutFormSubmit("Upload");
		echo "</div>";
		field::PutFormEnd();

		$Folder = $_REQUEST["Folder"];
		if ($Folder)
		{
			$Cell  = "";
			$Cell .= "<h2>".DBGet(WEB_DB, "SELECT description FROM mime_icons WHERE category = '".$Folder."'")."</h2>";
			$Cell .= "<a href=\"".FILE_BROWSER."\">";
			$Cell .= "<img src=\"".FILE_BROWSER."&FileIcon=home\" />";
			$Cell .= "<br />";
			$Cell .= "&lt;&lt; Home";
			$Cell .= "</a>";
			echo $Cell;
		}
		else
		{
			$EXT = array();
			$Q = new DBQuery;
			$Q->Run(FILE_DB, "SELECT distinct(ext) FROM file_items");
			while ($Q->Next())
				$EXT[] = $Q->GetField("ext");
			$Q->EndQuery;
			$Query  = "";
			$Query .= "SELECT mi.description AS folders, mi.category ";
			$Query .= "FROM mime_types mt, mime_icons mi ";
			$Query .= "WHERE mt.extension in ('".implode("', '", $EXT)."') ";
			$Query .= "AND mt.mime_icon = mi.category ";
			$Query .= "GROUP BY mt.mime_icon ";
			$Query .= "ORDER BY mi.description ";
			$Q->Run(WEB_DB, $Query);
			$T = new Table;
			while ($Q->Next())
			{
				$Cell  = "";
				$Cell .= "<a href=\"".FILE_BROWSER."&Folder=".$Q->GetField("category")."\">";
				$Cell .= "<img src=\"".FILE_BROWSER."&FileIcon=folder\" />";
				$Cell .= "<br />";
				$Cell .= $Q->GetField("folders");
				$Cell .= "</a>";
				$T->AddData($Cell);
			}
			$T->Put();
			$Q->EndQuery;
		}
		echo "<hr />";

		$Q = new DBQuery;
        $Q->Run(FILE_DB, "SELECT file_id, title, mime, ext FROM file_items ORDER BY title");
		$i=0;
		$T = new Table;
		$ThumbOn = "onmouseover=\"thu_XXXX.style.visibility='visible'\"";
        $ThumbOff = "onmouseout=\"this.style.visibility='hidden'\"";
		$ThumbStyle = "style=\"position: absolute; margin-left: -50px; margin-top: -50px; visibility: hidden;\" id=\"thu_XXXX\"";
		while ($Q->Next())
		{
			if ($Folder)
			{
				$ThisFile = DBGet(WEB_DB, "SELECT mime_icon FROM mime_types WHERE extension = '".$Q->GetField("ext")."'");
				$Display = ($ThisFile == $Folder);
			}
			else
				$Display = 1;

			if ($Display)
			{
				$WEB = DBGet(WEB_DB, "SELECT mime_icon = 'web' FROM mime_types WHERE mime_type = '".addslashes($Q->GetField("mime"))."'");
				$i++;
				$ID = $Q->GetField("file_id");
				$Cell  = "";
				$Cell .= $Q->GetField("title");
				$Cell .= "<br />";
				$Cell .= "<a href=\"Javascript:FilePopup('".FILE_BROWSER."&FileShow=".$ID."')\">";
				if ($Folder && $WEB)
					$Cell .= "<img src=\"".FILE_BROWSER."&FileThumb=".$ID."\" />";
				else
					$Cell .= "<img src=\"".FILE_BROWSER."&FileEXT=".$Q->GetField("ext")."\" />";
				$Cell .= "</a>";
				$Cell .= "<br />";
				$Cell .= "<a href=\"".files::url()."&FileEdit=".$ID."\">";
				$Cell .= "<img src=\"".FILE_BROWSER."&FileIcon=edit\" />";
				$Cell .= "</a>";
				if ($WEB)
				{
					if (!$Folder)
					{
						$Cell .= "<img src=\"".FILE_BROWSER."&FileIcon=view\" ".str_replace("XXXX", $ID, $ThumbOn)." />";
						$Cell .= "<a href=\"".files::url()."&FileEdit=".$ID."\">";
						$Cell .= "<img src=\"".FILE_BROWSER."&FileThumb=".$ID."\" ".$ThumbOff." ".str_replace("XXXX", $ID, $ThumbStyle)." />";
						$Cell .= "</a>";
					}
				}
				else
					$Cell .= "<img src=\"".FILE_BROWSER."&FileIcon=space\" />";
				$Message = "Are you sure you want to permanently delete this file?";
				$Cell .= "<a href=\"".files::url()."&FileDelete=".$ID."\" onclick=\"return confirm('".$Message."')\">";
				$Cell .= "<img src=\"".FILE_BROWSER."&FileIcon=delete\" alt=\"Delete\" />";
				$Cell .= "</a>";

				$T->AddData($Cell);
				if (($Folder && $WEB && $i == 5) || ($i == 10))
				{
					$T->EndRow();
					$i=0;
				}
			}
		}
		$T->Put();
		$Q->EndQuery;
	}

	function icon()
	{
		if ($_REQUEST["FileEXT"])
		{
			$Query = "SELECT mime_icon FROM mime_types WHERE extension = '".addslashes($_REQUEST["FileEXT"])."'";
			$Category = DBGet(WEB_DB, $Query);
		}
		else
			$Category = $_REQUEST["FileIcon"];
		if ($Category == "")
			$Category = "unknown";
		$Query = "SELECT icon FROM mime_icons WHERE category = '".$Category."'";
		$Icon = DBGet(WEB_DB, $Query);
		if (!strlen($Icon))
		{
			$Query = "SELECT icon FROM mime_icons WHERE category = 'warning'";
			$Icon = DBGet(WEB_DB, $Query);
		}

		Header("Content-type: image/png");
		echo base64_decode($Icon);
		exit;
	}

	function thumb($Size = 100, $Clean = 0)
	{
		$FileID = $_REQUEST["FileThumb"];
		$RAW = DBGet(FILE_DB, "select data from file_items where file_id = ".$FileID);
		$RAW = gzuncompress(base64_decode($RAW));

		$SRC = imagecreatefromstring ($RAW);
		$W1 = imagesx($SRC);
		$H1 = imagesy($SRC);
		if ($W1 > $H1)
		{
			$H2 = round($H1 / ($W1 / $Size));
			$W2 = $Size;
			$XS = 0;
			$YS = round(($Size - $H2) / 2);
		}
		else
		{
			$W2 = round($W1 / ($H1 / $Size));
			$H2 = $Size;
			$XS = round(($Size - $W2) / 2);
			$YS = 0;
		}

		if ($Clean)
		{
			$TGT = imagecreatetruecolor($W2, $H2);
			imagecopyresampled($TGT, $SRC, 0, 0, 0, 0, $W2, $H2, $W1, $H1);
		}
		else
		{
			$TGT = imagecreatetruecolor($Size, $Size);
			$Fill = imagecolorallocate($TGT, 255,  255,  255);
			imagefill($TGT, 1, 1, $Fill);
			imagecopyresampled($TGT, $SRC, $XS, $YS, 0, 0, $W2, $H2, $W1, $H1);
		}
		Header("Content-type: image/png");
		imagepng ($TGT);
		imagedestroy($SRC);
		imagedestroy($TGT);
		exit;
	}

	function save()
	{
		// validate the provided file..
		if ($Error = files::ValidateFile())
		{
			// create the redirection url
			$URL = files::url();

			// append the error..
			$URL .= "&Error=".urlencode($Error);
			
			// redirect
			Header("Location: ".$URL);
			exit;
		}
		
		$INQ = new DBInsert;
		$INQ->Save("title", $_REQUEST["FileTitle"]);
		$INQ->Save("updated", date("Y-m-d H:i:s"));

		if ($_FILES["FileData"]["size"])
		{
			$EXT = array_pop(explode(".", $_FILES["FileData"]["name"]));
			$fp = fopen($_FILES["FileData"]["tmp_name"], "r");
			$File = "";
			while (!feof($fp))
				$File .= fgets($fp, 4096);
			fclose($fp);

			$Query = "SELECT mime_type FROM mime_types WHERE extension = '".addslashes($EXT)."'";
			$MIME = DBGet(WEB_DB, $Query);
			if ($MIME)
			{
				$INQ->Save("mime", $MIME);
				$INQ->Save("data", base64_encode(gzcompress($File)));
				$INQ->Save("ext", $EXT);
			}
		}
		if ($_REQUEST["FileID"])
		{
			$INQ->SaveWhere("file_id", $_REQUEST["FileID"]);
			$INQ->Run(FILE_DB, "file_items", "UPDATE");
		}
		else
			$INQ->Run(FILE_DB, "file_items", "INSERT");
		$INQ->EndQuery();
	}

	function show()
	{
		$FileID = $_REQUEST["FileShow"];
		
		// get contents of file
		$Data = files::getfilecontents($FileID);

		Header("Content-type: ".$Data["mime"]);
		echo gzuncompress(base64_decode($Data["data"]));
		exit;
	}

	function delete()
	{
		$FileID = $_REQUEST["FileDelete"];
		DB_DoQuery(FILE_DB, "DELETE FROM file_items WHERE file_id = ".$FileID);
		Header("Location: ".files::url());
		exit;
	}

	function url()
	{
		if ($_REQUEST["Folder"])
			return(FILE_BROWSER."&Folder=".$_REQUEST["Folder"]);
		else
			return(FILE_BROWSER);
	}

	// display() - function to output the contents of a particular file (for inclusion in img tags etc)
	function display($FileID)
	{
		// get contents of file
		$Data = files::getfilecontents($FileID);
		header("Content-Type: ".$Data["mime"]);
		echo gzuncompress(base64_decode($Data["data"]));
		exit;
	}

	// download() - function to force the download of a particular file
	function download($FileID)
	{
		// get contents of file
		$Data = files::getfilecontents($FileID);
		$FileData = gzuncompress(base64_decode($Data["data"]));
		$FileSize = strlen($FileData);
		header('Content-Description: File Transfer'); 
		header("Content-Type: ".$Data["mime"]);
		header("Content-Length: ".$FileSize);
		$Title = $Data["title"];
		$Title = str_replace(array("\'","\"","\\","/"), "", $Title);
		$Title = str_replace(" ", "_", $Title);
		header("Content-Disposition: attachment; filename=\"".$Title.".".$Data["ext"]."\"");
		echo $FileData;
		exit;
	}

	// getfilecontents() - function to return the details for the required file
	function getfilecontents($FileID)
	{
		$Data = DBRow(FILE_DB, "SELECT * FROM file_items WHERE file_id = ".$FileID);
		return ($Data);
	}

	// ValidateFileBrowser - function to determine if the filebrowser define is set..
	function ValidateFileBrowser()
	{
		if (!defined("FILE_BROWSER"))
		{
			exit("FILE_BROWSER definition has not been provided. Please provide this definition so that the File Management Interface can be successfully implemented.");
		}
	}

	// ValidateFile - function to validate an uploaded file..
	function ValidateFile()
	{
		// we wish to carry out 4 checks:
		// 1. is there actually a file?
		// 2. does the file have an extension?
		// 3. if the provided extension is in the extension table in WEB_DB
		// 4. if the provided extension is ok with the FILE_ALLOW and FILE_DENY defines..

		// set some variables needed for comparisons when editing..
 		$Editing = $_REQUEST["FileID"];
		$EditFile = 0;

		// check 1. is there a file?
		if (!$_FILES["FileData"]["size"] && !$Editing)
			return ("No file found. Please upload a valid file.");
		else if ($_FILES["FileData"]["size"] && $Editing)
			$EditFile = 1;

		// explode the file name for future tests
		$FileNameParts = explode(".", $_FILES["FileData"]["name"]);

		
		// check 2. does the file have an extension?
		if (count($FileNameParts) <= 1 && (!$Editing || ($Editing && $EditFile)))
			return ("The provided file does not contain an extension. Please provide a valid extension.");

		// get the file's extension
		$FileExtension = array_pop($FileNameParts);

		// check 3. does the file extension exist within the extensions/mime_types table?
		$Qry = "SELECT COUNT(*) AS num FROM mime_types WHERE extension = '".$FileExtension."'";
		$Found = DBGet(WEB_DB, $Qry);
		if (!$Found && (!$Editing || ($Editing && $EditFile)))
			return ("The provided file contains an unknown file extension.");


		// check 4. does the file's extension marry up with permission guidelines?
		if (defined("FILE_ALLOW") && FILE_ALLOW != "" && (!$Editing || ($Editing && $EditFile)))
		{
			$AllowParts = explode("|", FILE_ALLOW);
			if (!in_array($FileExtension, $AllowParts))
				return ("The provided file is not included on the list of allowed file types. These file types are: ".implode(", ", $AllowParts).".");
		}
		else if (defined("FILE_DENY") && FILE_DENY != "" && (!$Editing || ($Editing && $EditFile)))
		{
			$DenyParts = explode("|", FILE_DENY);
			if (in_array($FileExtension, $DenyParts))
				return ("The provided file is of a type that is not allowed within this system. These file types are: ".implode(", ", $DenyParts).".");
		}
	}
}

?>

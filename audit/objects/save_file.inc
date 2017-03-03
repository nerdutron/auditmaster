<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: about.inc 8 2006-12-10 05:06:30Z scott                           $ //
//                                                                          //
// 	Object Model															// 
//  Copyright (C) Authors listed in AUTHORS.txt                             //
//                                                                          //
//  This program is free software; you can redistribute it and/or modify    //
//  it under the terms of the GNU General Public License as published by    //
//  the Free Software Foundation; either version 2 of the License, or       //
//  (at your option) any later version.                                     //
//                                                                          //
//  This program is distributed in the hope that it will be useful,         //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of          //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           //
//  GNU General Public License for more details.                            //
//                                                                          //
//  You should have received a copy of the GNU General Public License       //
//  along with this program; if not, write to the Free Software             //
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

require_once(PHPLIB_DIR."variable.inc");
require_once(ToIncludes("audit")."audit.inc");

class save_file extends variable
{
	function __construct()
	{
		parent::__construct();

		if (!empty($_FILES))
		{
			$tempFile = $_FILES['Filedata']['tmp_name'];
			$targetPath = $_SERVER['DOCUMENT_ROOT'] . $_REQUEST['folder'] . '/';
			$targetFile =  str_replace('//','/',$targetPath) . $_FILES['Filedata']['name'];
   
			move_uploaded_file($tempFile,$targetFile);
			echo "1";

			//$this->MakeL("OperatorID", "AuditID");
			$this->MakeL("AuditID");

			// Set params
			$Data = array();

			$Data["file_data"] = get_file_contents($targetFile);
			$Data["file_name"] = $_FILES["Filedata"]["name"];
			$Data["file_mime"] = $_FILES["Filedata"]["type"];
			$Data["file_size"] = size_of($Data["file_data"]);
			$Data["audit_id"] = $this->AuditID;

			// Save
			$Audit = new audit($this->AuditID);
			$Audit->SaveFile($Data);
		}
		else

		exit();
	}
}

?>

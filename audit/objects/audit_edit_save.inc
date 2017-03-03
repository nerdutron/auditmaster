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

require_once(ToObjects("audit")."audit_edit.inc");
require_once(ToIncludes("audit")."audit_business.inc");

class audit_edit_save extends audit_edit
{
	function __construct()
	{
		parent::__construct();

		$this->MakeL("FormData");

		if ($this->FormData["Step"])
			$this->Redir($this->URL("audit|audit_main", array("OperatorID" => $this->OperatorID)));

		if ($this->FormData["date_due"])
		{
			$this->FormData["date_due"] = date("Y-m-d", strtotime(str_replace("/", "-", $this->FormData["date_due"])));
		}

		if ($this->FormData["date_completed"])
		{
			$this->FormData["date_completed"] = date("Y-m-d", strtotime(str_replace("/", "-", $this->FormData["date_completed"])));
		}

		$this->FormData["operator_id"] = $this->OperatorID;

		$this->FormData["audit_status"] = audit_business::GetStatus($this->FormData);

		if ($this->Audit->Save($this->FormData))
		{
			$this->Message = "Details Saved";
			$this->Redir($this->URL("audit|audit_main", array("OperatorID" => $this->OperatorID)));
		}
		else
			$this->Message = "Error: ".$this->Audit->Error;
	}

	function PageContent($Message = "")
	{
		parent::PageContent($this->Message);
	}
}

?>

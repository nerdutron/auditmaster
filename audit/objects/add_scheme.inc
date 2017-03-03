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

require_once(ToObjects("audit")."audit_main.inc");
require_once(ToIncludes("audit")."audit_business.inc");

class add_scheme extends audit_main
{
	function __construct()
	{
		parent::__construct();

		$this->MakeL("AddScheme", "AddSchemeDate");

		$Audit = new audit_business($this->User, $this->OperatorID);

		if (trim($this->AddSchemeDate))
		{
			$Date = date("Y-m-d", strtotime(str_replace("/", "-", $this->AddSchemeDate)));
		}

		//$Audit->AddAudit($SchemeID, $this->OperatorID);
		$Audit->SchemeSignup($this->AddScheme, $Date);

/*
		echo "<tr><td>".field::GetCheck("Scheme[]", 0, $this->AddScheme);
		echo "</td><td>".scheme::GetSchemeName($this->AddScheme);
		echo "</td><td>".$this->AddSchemeDate;
		echo "</td><td></td><td></td></tr>";
*/
//				$T->AddData(tools::GetStageDesc($this->AuditBusiness->GetNextStage($Scheme["scheme_id"], $Dummy)));
//				$T->AddData(date(DATE_FORMAT, strtotime($Scheme["expiry_date"])));

		exit();
	}
}

?>
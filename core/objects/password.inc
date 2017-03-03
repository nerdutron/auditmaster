<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: password.inc 8 2006-12-10 05:06:30Z scott                        $ //
//                                                                          //
//  Sanctuary Faith Community Management Software                           //
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

require_once(ToObjects("core")."secure.inc");

class password extends secure
{
	function __construct()
	{
		parent::__construct();
	}

	function PageContent()
	{
		echo "<h3>Change Password</h3>";

		$Warn  = "";
		$Warn .= "<div class=\"alert\">";
		$Warn .= "<p><b>Warning:</b></p>";
		$Warn .= "<p>Changing the password will affect <i>all</i> users who share this password.</p>";
		$Warn .= "<p>Please use with caution.</p>";
		$Warn .= "</div>";

		$T = new CWTable;

		$T->AddHead("Old Password:");
		$T->AddData(field::GetPassword("Old", "", 16, 16));

		$T->AddData($Warn, 1, 4);

		$T->EndRow();

		$T->AddHead("New Password:");
		$T->AddData(field::GetPassword("New1", "", 16, 16));
		$T->EndRow();

		$T->AddHead("Confirm New:");
		$T->AddData(field::GetPassword("New2", "", 16, 16));
		$T->EndRow();

		$T->AddData(field::GetFormSubmit("Change Password"), 2);
		$T->EndRow();

		field::PutFormStart($this->URL("password_save"));
		$T->Put();
		field::PutFormEnd();
	}
}

?>

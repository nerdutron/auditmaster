<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: users.inc 7 2006-12-10 05:02:18Z scott                        $    //
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

require_once(ToObjects("core")."system_administration.inc");

class users extends system_administration
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "Current Users";
	}

	function PageContent()
	{
		$Query  = "";
		$Query .= "SELECT ";
		$Query .= "sp.user_id, sp.name, sp.email, ";
		$Query .= "usc.session_start, usc.session_expires,  ";
		$Query .= "a1.account_name AS account, ";
		$Query .= "a2.account_name AS master ";
		$Query .= "FROM user_session_current usc, ";
		$Query .= "account a1, ";
		$Query .= "account a2, ";
		$Query .= "security_public sp ";
		$Query .= "WHERE usc.user_id = sp.user_id ";
		$Query .= "AND sp.account_id = a1.account_id ";
		$Query .= "AND a1.master_account_id = a2.account_id ";

		echo "<h2>Current Users</h2>";

		$T = new CWTable;
		$T->AddHead("Name");
		$T->AddHead("Email");
		$T->AddHead("Account");
		$T->AddHead("Division");
		$T->AddHead("Logged In");
//		$T->AddHead("session_expires");
		$T->EndRow();

		$T->AlternateMouseOver();
		$Q = new DBQuery;
		$Q->Run(ACC_DB, $Query);
		while ($Q->Next())
		{
			$User = $Q->GetArray();
			$T->AddData($Q->GetField("name"));
			$T->AddData($Q->GetField("email"));
			$T->AddData($Q->GetField("master"));
			$T->AddData($Q->GetField("account"));
			$T->AddData(date("d/m/Y H:ia", CreateTimeStamp($Q->GetField("session_start"))));
//			$T->AddData($Q->GetField("session_expires"));
			$T->EndRow();
		}
		$Q->EndQuery();
		$T->Put();
	}
}

?>

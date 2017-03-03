<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: denied.inc 77 2007-08-03 06:44:04Z rod                           $ //
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

class denied extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "[Sanctuary] Access Error";
	}
	
	function CreditWorksContact()
	{
		echo "<p>Please contact your CreditWorks Account Manager to discuss gaining access.</p>";
	}

	function ContactAccount()
	{
		$Query  = "";
		$Query .= "SELECT name, email ";
		$Query .= "FROM security_public ";
		$Query .= "WHERE account_id = ".$this->User->Get("master_account_id")." ";
		$Query .= "AND active = 1 ";
		$Query .= "AND user_type = 3";
		$Q = new DBQuery;
		$Q->Run(ACC_DB, $Query);
		if ($Q->NumRows())
		{
			echo "<p>Please contact your Account Administrator (below) to gain access.</p>";
			echo "<ul>";
			while ($Q->Next())
			{
				echo "<li>";
				echo $Q->GetField("name");
				echo " <a href=\"mailto:";
				echo $Q->GetField("email");
				echo "\">";
				echo $Q->GetField("email");
				echo "</a>";
				echo "</li>";
			}
			echo "</ul>";
		}
		else
			$this->CreditWorksContact();
		$Q->EndQuery();
	}

	function ContactDivision()
	{
		$Query  = "";
		$Query .= "SELECT u.name, u.email ";
		$Query .= "FROM security_public u, account a ";
		$Query .= "WHERE a.master_account_id = ".$this->User->Get("master_account_id")." ";
		$Query .= "AND a.account_id = u.account_id ";
		$Query .= "AND u.active = 1 ";
		$Query .= "AND u.user_type = 2";
		$Q = new DBQuery;
		$Q->Run(ACC_DB, $Query);
		if ($Q->NumRows())
		{
			echo "<p>Please contact your Division Administrator (below) to gain access.</p>";
			echo "<ul>";
			while ($Q->Next())
			{
				echo "<li>";
				echo $Q->GetField("name");
				echo " <a href=\"mailto:";
				echo $Q->GetField("email");
				echo "\">";
				echo $Q->GetField("email");
				echo "</a>";
				echo "</li>";
			}
			echo "</ul>";
		}
		else
			$this->ContactAccount();
		$Q->EndQuery();
	}

	function PageContent()
	{
		echo "<h2>";
		switch($this->GetV("Reason"))
		{
			case 3:
				echo "You have tried to access an unpublished service.";
				break;
			case 4:
				echo "You do not have access to this Service.";
				break;
			case 5:
				echo "You do not have sufficient Administration privelegaes.";
				break;
			case 6:
				echo "You have implicitly been denied access to this service.";
				break;
			case 7:
			case 8:
				echo "You have not implicitly been granted access to this service.";
				break;
		}
		echo "</h2>";

//		switch ($this->User->Get("user_type"))
		switch (0)
		{
			case 0:
			case 1:
				$this->ContactDivision();
				break;
			case 2:
				$this->ContactAccount();
				break;
			case 3:
				$this->CreditWorksContact();
				break;
			case 4:
				break;
		}
	}
}

?>

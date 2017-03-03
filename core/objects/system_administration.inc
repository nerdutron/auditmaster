<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: system_administration.inc 10 2006-12-10 12:58:21Z scott          $ //
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

class system_administration extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "System Administration";

		$this->MenuLocation("SysAdmin", "system_administration");
		$this->MainMenu = array();
		$this->MainMenu["Staff"] = "staff";
		$this->MainMenu["Users"] = "users";
		$this->MainMenu["Accounts"] = "accounts";
		$this->MainMenu["Services"] = "services_list";
		$this->MainMenu["Countries"] = "countries_list";
		$this->MainMenu["Outages"] = "outages";
		$this->MainMenu["Files"] = "file_manager";
	}

	function PageContent()
	{
		echo "Welcome to System Administration";
	}
}

?>

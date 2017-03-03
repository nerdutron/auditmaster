<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: menu.inc 9 2006-12-10 05:17:10Z scott                            $ //
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

class menu_builder
{
	function __construct()
	{
		$this->reset_menu_item();
		$this->Menus = array();
		$this->Custom = array();
	}

	function define_menus($Menu)
	{
		$this->Menus[$Menu] = array();
	}

	function add_to_menu($Menu)
	{
		$this->Menus[$Menu][] = $this->Current;
		$this->reset_menu_item();
	}

	function get_menu($Menu)
	{
		return($this->Menus[$Menu]);
	}

	function reset_menu_item()
	{
		$this->Current = array();
	}

	function entry_object($Title, $Object)
	{
		$this->Current["title"] = $Title;
		$this->Current["object"] = $Object;
		$this->Current["address"] = "";
	}

	function entry_address($Title, $Address)
	{
		$this->Current["title"] = $Title;
		$this->Current["object"] = "";
		$this->Current["address"] = $Address;
	}

	function entry_menu($Menu)
	{
		$this->Current["menu"] = $this->Menus[$Menu];
		$this->Menus[$Menu] = array();
	}

	function entry_params($Params)
	{
		$this->Current["params"] = $Params;
	}

	function create()
	{
		$this->define_menus("MAIN");
		$this->define_menus("SUB1");
		$this->define_menus("SUB2");

		$this->add_creditworks_menu();
		$this->add_pinnacle_menu();
		$this->add_asic_menu();
		$this->add_baycorp_menu();
		$this->add_users_menu();
		$this->add_admin_menu();
	}

	function add_creditworks_menu()
	{
		$this->entry_object("CreditWorks", "welcome");
		$this->add_to_menu("SUB1");
		$this->entry_object("Change Password", "password");
		$this->add_to_menu("SUB1");
		$this->entry_object("Check Usage", "usage");
		$this->entry_params(array("Type"=>"Search"));
		$this->add_to_menu("SUB1");
		$this->entry_object("Check Trans", "usage");
		$this->entry_params(array("Type"=>"Trans"));
		$this->add_to_menu("SUB1");
		$this->entry_address("Logout", "index.php?Action=secure_logout\" onclick=\"return confirm('Are you sure you wish to logout?')");
		$this->add_to_menu("SUB1");
		$this->entry_object("Subscribers", "secure");
		$this->entry_menu("SUB1");
		$this->add_to_menu("MAIN");
	}

	function set_custom($Location, $Data)
	{
		$this->Custom[$Location] = $Data;
	}

	function add_custom($Location, $Menu)
	{
		if (is_array($this->Custom[$Location]))
		{
			foreach ($this->Custom[$Location] as $Item)
			{
				$this->entry_address($Item[0], $Item[1]);
				$this->add_to_menu($Menu);
			}
		}
	}

	function add_pinnacle_menu()
	{
		$this->entry_object("Dashboard", "cgrs|dashboard");
		$this->add_to_menu("SUB1");

		$this->add_custom("Pinnacle", "SUB1");

		$this->entry_object("Summary", "cgrs|summary");
		$this->add_to_menu("SUB1");
		$this->entry_object("Enquiries", "cgrs|enquiries");
		$this->add_to_menu("SUB1");
		$this->entry_object("Credit Limits", "cgrs|credit_limits");
		$this->add_to_menu("SUB1");
		$this->entry_object("Reports", "cgrs|reports");
		$this->add_to_menu("SUB1");
		$this->entry_object("Report Sets", "cgrs|grouping");
		$this->add_to_menu("SUB1");
		$this->entry_object("Data Upload", "cgrs|upload");
		$this->add_to_menu("SUB1");
		$this->entry_object("Group Limits", "cgrs|policies_group_limits");
		$this->add_to_menu("SUB2");
		$this->entry_object("Terms of Trade", "cgrs|policies_trade_profiles");
		$this->add_to_menu("SUB2");
		$this->entry_object("Policy Rules &gt;", "cgrs|policies_group_limits");
		$this->entry_menu("SUB2");
		$this->add_to_menu("SUB1");
		$this->entry_object("Matching", "cgrs|matching");
		$this->add_to_menu("SUB1");
		$this->entry_object("Release Notes", "cgrs|release_notes");
		$this->add_to_menu("SUB1");
		$this->entry_object("Pinnacle", "cgrs|base");
		$this->entry_menu("SUB1");
		$this->add_to_menu("MAIN");
	}

	function add_asic_menu()
	{
		$this->entry_object("Organisation Name Search", "asic|ons_search");
		$this->add_to_menu("SUB1");
		$this->entry_object("Company Extract", "asic|extract_search");
		$this->add_to_menu("SUB1");
		$this->entry_object("Company Document Listing", "asic|cdl_search");
		$this->add_to_menu("SUB1");
		$this->entry_object("Document Image", "asic|docimage_search");
		$this->add_to_menu("SUB1");
		$this->entry_object("Personal Name Browse", "asic|pns_search");
		$this->add_to_menu("SUB1");
		$this->entry_object("ASIC", "asic|base");
		$this->entry_menu("SUB1");
		$this->add_to_menu("MAIN");
	}

	function add_baycorp_menu()
	{
		$this->entry_object("Company Search", "b2bay|co_search");
		$this->add_to_menu("SUB1");
		$this->entry_object("Business Search", "b2bay|bus_search");
		$this->add_to_menu("SUB1");
		$this->entry_object("MultiPower Search", "b2bay|multipower_search");
		$this->add_to_menu("SUB1");
		$this->entry_object("Baycorp", "b2bay|base");
		$this->entry_menu("SUB1");
		$this->add_to_menu("MAIN");
	}

	function add_users_menu()
	{
		$this->entry_object("Users", "accounts_users");
		$this->add_to_menu("MAIN");
	}

	function add_admin_sub()
	{
		$this->entry_object("View", "accounts_view");
		$this->add_to_menu("SUB2");
		$this->entry_object("IP", "accounts_ip");
		$this->add_to_menu("SUB2");
		$this->entry_object("Logo", "accounts_logo");
		$this->add_to_menu("SUB2");
		$this->entry_object("Matter Ref", "accounts_matter_ref");
		$this->add_to_menu("SUB2");
		$this->entry_object("Divisions", "accounts_divisions");
		$this->add_to_menu("SUB2");
		$this->entry_object("Services", "accounts_services");
		$this->add_to_menu("SUB2");
		$this->entry_object("Pinnacle Profile", "accounts_cgrs");
		$this->add_to_menu("SUB2");
		$this->entry_object("Pinnacle Loader", "accounts_cgrs_loader");
		$this->add_to_menu("SUB2");
		$this->entry_object("Pinnacle Menus", "accounts_pinnacle_menus");
		$this->add_to_menu("SUB2");
	}

	function add_admin_menu()
	{
		$this->entry_object("Staff", "staff");
		$this->add_to_menu("SUB1");
		$this->entry_object("Users", "users");
		$this->add_to_menu("SUB1");
		$this->add_admin_sub();
		$this->entry_object("Accounts &gt;", "accounts");
		$this->entry_menu("SUB2");
		$this->add_to_menu("SUB1");
		$this->entry_object("Services", "services_list");
		$this->add_to_menu("SUB1");
		$this->entry_object("Countries", "countries_list");
		$this->add_to_menu("SUB1");
		$this->entry_object("Dashlets", "dashlets_list");
		$this->add_to_menu("SUB1");
		$this->entry_object("Outages", "outages");
		$this->add_to_menu("SUB1");
		$this->entry_object("Files", "file_manager");
		$this->add_to_menu("SUB1");
		$this->entry_object("SysAdmin", "system_administration");
		$this->entry_menu("SUB1");
		$this->add_to_menu("MAIN");
	}
}

// $MB = new menu_builder;
// $MB->create();
// $MAIN_MENU = $MB->get_menu("MAIN");

/*
//        if (date("d") >= 7)
//            $this->MainMenu["Late Payers"] = "cgrs|news_30_days";
*/

?>

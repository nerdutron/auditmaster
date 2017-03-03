<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: header.txt 13 2006-12-10 13:10:12Z scott                         $ //
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

//require_once(ToObjects("core").CONTAINER_PROJECT.".inc");
require_once(ToObjects("core")."secure.inc");

class base extends secure
{
	var $class_path = "core";

	function __construct()
	{
		parent::__construct();
		$this->Title = "[Sanctuary] Groups";
		$this->MenuLocation("Sanctuary", "core|secure");
		$this->MainMenu = array();
		$this->MainMenu["Add Group Type"] = "groups|add_group_type";
		$this->MainMenu["Add Group"] = "groups|add_group";
		$this->MainMenu["List Groups"] = "groups|list_groups";
	}

	function PageContent()
	{
		echo "<h2>Welcome to the Sanctuary Groups Menu</h2>";
	}
}

?>

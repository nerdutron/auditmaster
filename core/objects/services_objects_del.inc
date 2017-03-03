<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: services_objects_del.inc 8 2006-12-10 05:06:30Z scott            $ //
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

require_once(ToObjects("core")."services_objects.inc");

class services_objects_del extends services_objects
{
	function __construct()
	{
		parent::__construct();
		$Query = "DELETE FROM service_objects WHERE secure_object = '".addslashes($this->Object)."'";
		DB_DoQuery(ACC_DB, $Query);
		$this->Redir($this->URL("services_objects", array("ViewID"=>$this->ViewID)));
	}
}

?>
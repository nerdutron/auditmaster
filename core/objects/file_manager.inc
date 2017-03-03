<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: file_manager.inc 8 2006-12-10 05:06:30Z scott                    $ //
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

require_once(OBJECTS_DIR."system_administration.inc");
require_once(PHPLIB_DIR."files.inc");

class file_manager extends system_administration
{
	function __construct()
	{
		$this->Title = "File Manager";
		parent::__construct();
		files::head();
	}

	function PageContent()
	{
		files::body();
	}
}

?>

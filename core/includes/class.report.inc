<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: class.report.inc 9 2006-12-10 05:17:10Z scott                    $ //
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
//////////////////////////////////////////////////////////////////////////////

include_once(PHPLIB_DIR."pdf/class.acrobat.inc");

class report extends ACROBAT
{
	function __construct()
	{
		parent::__construct();

		$this->HeaderData = array();

		$this->HeaderCover = "none";
		$this->HeaderRest = "none";

		$this->Margins = array();
		$this->Margins["header"]["none"] = 60;
		$this->Margins["footer"]["none"] = 40;
	}

	function HeaderDataNone()
	{
	}

	function FooterDataNone()
	{
		$Y = $this->mm(10);
		$this->FullLine($Y);
		$this->TextRight("Page ".$this->FooterPageNum." of ".$this->FooterTotalPageNum, $this->ez["leftMargin"], $this->mm(6), 8);
	}
}

?>

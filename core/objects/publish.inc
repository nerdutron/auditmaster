<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id::                                                                  $ //
//                                                                          //
// Copyright (C) 2006 - 													//
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

require_once(ToObjects("core")."site.inc");

class publish extends site
{
	function __construct()
	{
		parent::__construct();
//		if (preference::get_reseller_preference("NON_PUBLIC_SITE") == "Y")
//			$this->Redir($this->URL("secure"));
	}
}

?>

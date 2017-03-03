<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."pdf/class.acrobat.inc");

class BASEPDF extends ACROBAT
{
	function __construct()
	{
		$this->InitBASEPDF();
	}

	function InitBASEPDF()
	{
		parent::__construct();

		$this->HeaderData = array();

		$this->HeaderCover = "cover";
		$this->HeaderRest = "page";

		$this->Margins = array();

		$this->Margins["header"]["cover"] = 30;
		$this->Margins["footer"]["cover"] = 30;

		$this->Margins["header"]["page"] = 60;
		$this->Margins["footer"]["page"] = 40;

		$this->Margins["header"]["none"] = 60;
		$this->Margins["footer"]["none"] = 40;

		$this->Margins["header"]["short"] = 100;
		$this->Margins["footer"]["short"] = 85;

	//	$this->CoverLogo = "/www/images/pdf_png/abr_logo.png";
	//	$this->HeadLogo = "/www/images/pdf_png/abr_logo_small.png";
	}

	//
	// page Headers
	//

	function HeaderDataCover()
	{
//		$I = getImageSize($this->CoverLogo);
//		$PNG["W"] = $I[0] / 3.5 * 2;
//		$PNG["H"] = $I[1] / 3.5 * 2;

//		$Max["W"] = 300;
//		$Max["H"] = 400;
//		$Min["W"] = 150;
//		$Min["H"] = 150;

//		$Rescale = 1;

//		if ($PNG["W"] > $PNG["H"])
//			$Sides = array("H", "W");
//		else
//			$Sides = array("W", "H");

//		if ($PNG[$Sides[0]] < $Min[$Sides[0]])
//			$Rescale = $Min[$Sides[0]] / $PNG[$Sides[0]];

//		for ($i=0;$i<sizeof($Sides);$i++)
//			if (($PNG[$Sides[$i]] * $Rescale) > $Max[$Sides[$i]])
//				$Rescale = $Max[$Sides[$i]] / ($PNG[$Sides[$i]] * $Rescale);

//		$PNG["W"] = $PNG["W"] * $Rescale;
//		$PNG["H"] = $PNG["H"] * $Rescale;

//		$this->addPngFromFile($this->CoverLogo,
//							(($this->ez['pageWidth'] - $PNG["W"]) / 2),
//							(($this->ez['pageHeight'] / 2) + 30),
//							$PNG["W"], $PNG["H"]);
		$this->setLineStyle(1, 'butt', 'miter', array());
		$this->rectangle(30, 30, $this->ez['pageWidth'] - 60, $this->ez['pageHeight'] - 60);
		$this->ShowHeaderData($this->ez["leftMargin"], (($this->ez['pageHeight'] / 2) - 20), 1, "center");

		$BackupFooterData = $this->FooterData;
	 	$this->ShowFooterData(35, 35, 1);
		$this->FooterData = $BackupFooterData;

		$H = $this->ez['pageHeight'] - 30;
	}

	function HeaderDataPage()
	{
		$this->FullLine($this->ez['pageHeight'] - 40);
		$this->ShowHeaderData(30, ($this->ez['pageHeight'] - 10), 0, "right");
	}

	function HeaderDataShort()
	{
		$W = 0;
		$H = 66;
//		$this->addPngFromFile($this->HeadLogo,
//			$this->ez['leftMargin'],
//			($this->ez['pageHeight'] - $H - 10),
//			$W, $H);
		$this->FullLine($this->ez['pageHeight'] - $H - 15);
		$this->ShowHeaderData(150, ($this->ez['pageHeight'] - 10), 0, "right");
	}


	function HeaderDataNone()
	{
	}

	//
	// page Footers
	//

	function FooterDataCover()
	{
	}

	function FooterDataPage()
	{
		$this->FullLine($this->mm(10));
		$this->TextRight("Page ".$this->FooterPageNum." of ".$this->FooterTotalPageNum, $this->ez["leftMargin"], $this->mm(6), 8);
	}

	function FooterDataShort()
	{
		$this->FooterDataPage();
	}

	function FooterDataNone()
	{
	}
}

?>

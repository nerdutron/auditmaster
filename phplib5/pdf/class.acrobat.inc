<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."tools.inc");
require_once(PHPLIB_DIR."pdf/class.pdf.inc");
require_once(PHPLIB_DIR."pdf/class.ezpdf.inc");

class ACROBAT extends Cezpdf
{
	function __construct()
	{
		if (defined("PDF_IMG"))
			$this->WorkingPath = PDF_IMG;
		if (!isset($this->WorkingPath))
			$this->WorkingPath = "/tmp/";
		$this->Table = array();
		$this->Contents = array();

		$this->TitleStyle = array();

		$this->TitleStyle[1]["font"]		= "Helvetica";
		$this->TitleStyle[1]["colour"]		= array(0,0,0);
		$this->TitleStyle[1]["align"]		= "center";
		$this->TitleStyle[1]["size"]		= 24;
		$this->TitleStyle[1]["above"]		= 0;
		$this->TitleStyle[1]["below"]		= 1;
		$this->TitleStyle[1]["uppercase"]	= 0;
		$this->TitleStyle[1]["newpage"]		= 1;
		$this->TitleStyle[1]["tabs"]		= 0;
		$this->TitleStyle[1]["italic"]		= 0;
		$this->TitleStyle[1]["bold"]		= 1;

		$this->TitleStyle[2] = $this->TitleStyle[1];
		$this->TitleStyle[2]["align"]		= "left";
		$this->TitleStyle[2]["size"]		= 18;
		$this->TitleStyle[2]["above"]		= 0.5;
		$this->TitleStyle[2]["below"]		= 0.3;
		$this->TitleStyle[2]["newpage"]		= 0;

		$this->TitleStyle[3] = $this->TitleStyle[2];
		$this->TitleStyle[3]["size"]		= 14;

		$this->TitleStyle[4] = $this->TitleStyle[3];
		$this->TitleStyle[4]["size"]		= 12;

		$this->TitleIndexLevel = 0;

		$this->TitleIndexes = array();
		$this->TitleIndexes[1] = 0;
		$this->TitleIndexes[2] = 0;
		$this->TitleIndexes[3] = 0;
		$this->TitleIndexes[4] = 0;
	}

	function ResetObjectProperties()
	{
		$this->setLineStyle(1,'','',array());
		$this->setColor(0,0,0);
		$this->setStrokeColor(0,0,0);
	}

	function ChangeFont($Font)
	{
		$this->selectFont(PHPLIB_DIR."pdf/".$Font.'.afm');
	}

	function StartPDF($PaperSize = 'a4', $Orientation = 'portrait')
	{
		$this->Cezpdf($PaperSize, $Orientation);
		if (in_array($this->HeaderCover, array("cover", "nz_cover")))
			$this->ezNewPage();
		$this->ChangeFont('Helvetica');

		$T = $this->HeaderMargins("header", $this->HeaderRest);
		$B = $this->HeaderMargins("footer", $this->HeaderRest);
		$R = $this->HeaderMargins("right", $this->HeaderRest);
		$L = $this->HeaderMargins("left", $this->HeaderRest);
		if (!$T)
			$T = 30;
		if (!$B)
			$B = 30;
		if (!$R)
			$R = 30;
		if (!$L)
			$L = 30;
		$this->ezSetMargins($T,$B,$L,$R);
	}

	function PDFDownloadID($UserID, $TransTypeID = 0, $URL = "")
	{
//		$this->ProcessHeader();
//		$Document = $this->ezOutput();
//		$PC = new DocumentCache($UserID, $Document, "application/pdf", $TransTypeID);
//		if ($URL)
//			$PC->ChangePageViewer($URL);
//		return ($PC->GetDocumentID());
		echo "Requires page cache!";
	}

	function PDFDownloadLink($UserID, $TransTypeID = 0, $URL = "")
	{
//		$this->ProcessHeader();
//		$Document = $this->ezOutput();
//		$PC = new DocumentCache($UserID, $Document, "application/pdf", $TransTypeID);
//		if ($URL)
//			$PC->ChangePageViewer($URL);
//		return ($PC->GetDocumentLink());
		echo "Requires page cache!";
	}

	function PDFDownloadNow($UserID, $TransTypeID = 0, $URL = "")
	{
//		$this->ProcessHeader();
//		$Document = $this->ezOutput();
//		$PC = new DocumentCache($UserID, $Document, "application/pdf", $TransTypeID);
//		if ($URL)
//			$PC->ChangePageViewer($URL);
//		$PC->GetDocumentNow();
		echo "Requires page cache!";
	}

	function PDFGetData()
	{
		$this->ProcessHeader();
		$Document = $this->ezOutput(1);
		return ($Document);
	}

	function ProcessHeader()
	{
		$ID = $this->ezPages[1];
		$this->reopenObject($ID);
		$this->GetHeaderData($this->HeaderCover, 1);
		$this->GetFooterData($this->HeaderCover, 1, sizeof($this->ezPages));
		$this->closeObject($ID);

		for ($i=2;$i<=sizeof($this->ezPages);$i++)
		{
			$ID = $this->ezPages[$i];
			$this->reopenObject($ID);
			$this->GetHeaderData($this->HeaderRest, $i);
			$this->GetFooterData($this->HeaderRest, $i, sizeof($this->ezPages));
			$this->closeObject($ID);
		}
	}

	function DownloadPDF()
	{
		$this->ProcessHeader();
		$this->ezStream();
	}

	function HeaderMargins($Type, $Data)
	{
		if (isset($this->Margins) && is_array($this->Margins) && isset($this->Margins[strtolower($Type)]) && is_array($this->Margins[strtolower($Type)]) && isset($this->Margins[strtolower($Type)][strtolower($Data)]))
		  return ($this->Margins[strtolower($Type)][strtolower($Data)]);
	}

	function mm($mm)
	{
		// 1 inch = 25.4 millimetres
		// 1 inch =  72 units
		// therefore:
		//	72 units = 25.4 millimetres
		//	1 unit   = 25.4 / 72 millimetres
		// hence:
		$Scale = 25.4 / 72;

		if ($mm)
			return round(($mm / $Scale));
		else
			return(0);
	}

	function rgb($n)
	{
		// 256 shades = units 0-1
		// each part is 1/255 (0 is not a part and so is ignored_
		// therefore:
		//	units = shades * parts
		// hence
		//	units = shades * (1/255);
		$Scale = (1 / 255);

		return ($n * $Scale);
	}

	function hex($n)
	{
		return (hexdec($n));
	}

	function hexrgb($n)
	{
		return ($this->rgb($this->hex($n)));
	}

	function hexsplit($h)
	{
		$h = str_replace("#", "", $h);
		$hex = array();
		$hex[] = $this->hexrgb(substr($h,0,2));
		$hex[] = $this->hexrgb(substr($h,2,2));
		$hex[] = $this->hexrgb(substr($h,4,2));
		return($hex);
	}

	function CreateName()
	{
		return ($this->WorkingPath.CreateUniqueKey(16));
	}

	function addGifFromFile($ImageFilename, $X, $Y, $W="", $H="")
	{
		$RandomFile = $this->CreateName();

		$PNMFilename = $RandomFile.".pnm";
		$PNGFilename = $RandomFile.".png";

		system("giftopnm ".$ImageFilename." > ".$PNMFilename);
		system("pnmtopng ".$PNMFilename." > ".$PNGFilename);

		if (filesize($PNGFilename) != 0)
			$this->addPngFromFile($PNGFilename, $X, $Y, $W, $H);

		unlink($PNMFilename);
		unlink($PNGFilename);
	}

// Added by Craig 1/10/04 to add png from a variable
// Could have done this nicer by changing base class or copying and hacking base class
// code, but that could cause problems if base class is updated
	function addPngFromVariable($PngData, $X, $Y, $W="", $H="")
	{
		$RandomFile = $this->CreateName().".gif";
		
		$fp_gif = fopen($RandomFile, "w");
		fwrite($fp_gif, $PngData);
		fclose($fp_gif);

		$this->addPngFromFile($RandomFile, $X, $Y, $W, $H);

		unlink($RandomFile);
	}

	function addPngFromWeb($URL, $X, $Y, $W="", $H="")
	{
		$RandomFile = $this->CreateName().".gif";
		
//		$Data = fread(fopen($URL, "r"), 10000000);

		$Data = "";
		$Handle = fopen($URL, "rb");
		while (!feof($Handle))
			$Data .= fread($Handle, 8192);
		fclose($Handle);

		$fp_gif = fopen($RandomFile, "w");
		fwrite($fp_gif, $Data);
		fclose($fp_gif);

		$this->addPngFromFile($RandomFile, $X, $Y, $W, $H);

		unlink($RandomFile);
	}

	function addGifFromWeb($URL, $X, $Y, $W="", $H="")
	{
		$RandomFile = $this->CreateName().".gif";
		
//		$Data = fread(fopen($URL, "r"), 10000000);

		$Data = "";
		$Handle = fopen($URL, "rb");
		while (!feof($Handle))
			$Data .= fread($Handle, 8192);
		fclose($Handle);	

		$fp_gif = fopen($RandomFile, "w");
		fwrite($fp_gif, $Data);
		fclose($fp_gif);
		
		$this->addGifFromFile($RandomFile, $X, $Y, $W, $H);

		unlink($RandomFile);
	}

	function addJpegFromWeb($URL, $X, $Y, $W="", $H="")
	{
		$RandomFile = $this->CreateName().".jpeg";

//		$Data = fread(fopen($URL, "r"), 10000000);

		$Data = "";
		$Handle = fopen($URL, "rb");
		while (!feof($Handle))
			$Data .= fread($Handle, 8192);
		fclose($Handle);

		$fp_jpeg = fopen($RandomFile, "w");
		fwrite($fp_jpeg, $Data);
		fclose($fp_jpeg);

		$this->addJpegFromFile($RandomFile, $X, $Y, $W, $H);

		unlink($RandomFile);
	}

	function InsertImage($Method, $Source, $Type, $W, $H, $Justification = "center")
	{
		$Type			= strtolower($Type);
		$Method			= strtolower($Method);
		$Justification	= strtolower($Justification);
		if (($this->y - $this->ez["bottomMargin"]) < $H)
			$this->ezNewPage();
		switch($Justification)
		{
			case "left":
				$X = $this->ez["leftMargin"];
				break;
			case "right":
				$X = $this->ez["pageWidth"] - $this->ez["rightMargin"] = $W;
				break;
			default:
				$X = round((($this->ez["pageWidth"] - $this->ez["rightMargin"] - $this->ez["leftMargin"]) / 2) - ($W / 2)) + $this->ez["leftMargin"];
				break;
		}
		$Y = $this->y - $H;

		if (($Method == "web") and ($Type == "png"))
			$this->addPngFromWeb($Source, $X, $Y, $W, $H);
		if (($Method == "file") and ($Type == "png"))
			$this->addPngFromFile($Source, $X, $Y, $W, $H);
		if (($Method == "web") and ($Type == "gif"))
			$this->addGifFromWeb($Source, $X, $Y, $W, $H);
		if (($Method == "file") and ($Type == "gif"))
			$this->addGifFromFile($Source, $X, $Y, $W, $H);
		if (($Method == "web") and ($Type == "jpeg"))
			$this->addJpegFromWeb($Source, $X, $Y, $W, $H);
		if (($Method == "file") and ($Type == "jpeg"))
			$this->addJpegFromFile($Source, $X, $Y, $W, $H);

		$this->ezSetDy(0 - $H);
	}

	function AddHeaderData($Text, $Size = 10, $Cover = 0, $Font = "Helvetica", $R = 0, $G = 0, $B = 0)
	{
		$i = sizeof($this->HeaderData);
		$this->HeaderData[$i]["Text"] = $Text;
		$this->HeaderData[$i]["Size"] = $Size;
		$this->HeaderData[$i]["Font"] = $Font;
		$this->HeaderData[$i]["Cover"] = $Cover;
		$this->HeaderData[$i]["R"] = $this->rgb($R);
		$this->HeaderData[$i]["G"] = $this->rgb($G);
		$this->HeaderData[$i]["B"] = $this->rgb($B);
	}

	function ShowHeaderData($X, $Y, $Cover = 0, $Align = "center")
	{
		$this->changeFont("Helvetica");
		for ($i=0;$i<sizeof($this->HeaderData);$i++)
		{
			if ($this->HeaderData[$i]["Cover"] + $Cover)
			{
				$this->selectFont(PHPLIB_DIR."pdf/".$this->HeaderData[$i]["Font"].".afm");
				$this->setColor($this->HeaderData[$i]["R"], $this->HeaderData[$i]["G"], $this->HeaderData[$i]["B"]);
				$Y = round($Y - ($this->getFontHeight($this->HeaderData[$i]["Size"]) * 1.1));
				switch ($Align)
				{
					case "center":
					case "centre":
						$this->TextCenter($this->HeaderData[$i]["Text"], $X, $Y, $this->HeaderData[$i]["Size"]);
						break;
					case "right":
						$this->TextRight($this->HeaderData[$i]["Text"], $X, $Y, $this->HeaderData[$i]["Size"]);
						break;
					default:
						$this->TextLeft($this->HeaderData[$i]["Text"], $X, $Y, $this->HeaderData[$i]["Size"]);
						break;
				}
			}
		}
		$this->selectFont(PHPLIB_DIR."pdf/Helvetica.afm");
		$this->setColor(0,0,0);
	}

	function AddFooterData($Text, $Size = 10, $Cover = 0, $Font = "Helvetica", $R = 0, $G = 0, $B = 0)
	{
		$i = sizeof($this->FooterData);
		$this->FooterData[$i]["Text"] = $Text;
		$this->FooterData[$i]["Size"] = $Size;
		$this->FooterData[$i]["Font"] = $Font;
		$this->FooterData[$i]["Cover"] = $Cover;
		$this->FooterData[$i]["R"] = $this->rgb($R);
		$this->FooterData[$i]["G"] = $this->rgb($G);
		$this->FooterData[$i]["B"] = $this->rgb($B);
	}

	function AppendDisclaimer($Width, $Size)
	{
		$Lines = array();
		$Words = explode(" ", trim($this->Disclaimer));

		$this->AddFooterData(" ", $Size);

		$l = "";
		for ($w=0;$w<sizeof($Words);$w++)
		{
			if ($this->GetTextWidth($Size, trim($l." ".$Words[$w])) >= $Width)
			{
				if ($l)
					$this->AddFooterData($l, $Size);
				$l = "";
			}
			$l = trim($l." ".$Words[$w]);
		}
		if ($l)
			$this->AddFooterData($l, $Size);
	}

	function ShowFooterData($X, $Y, $Cover = 0, $Color = array())
	{
		for ($i=0;$i<sizeof($this->FooterData);$i++)
			if ($this->FooterData[$i]["Cover"] + $Cover)
				$Y = round($Y + ($this->getFontHeight($this->FooterData[$i]["Size"]) * 1.1));

		for ($i=0;$i<sizeof($this->FooterData);$i++)
			if ($this->FooterData[$i]["Cover"] + $Cover)
			{
				$this->selectFont(PHPLIB_DIR."pdf/".$this->FooterData[$i]["Font"].".afm");
				$this->setColor($this->FooterData[$i]["R"], $this->FooterData[$i]["G"], $this->FooterData[$i]["B"]);
				$Y = round($Y - ($this->getFontHeight($this->FooterData[$i]["Size"]) * 1.1));
				$this->addText($X, $Y, $this->FooterData[$i]["Size"], $this->FooterData[$i]["Text"],0,0,$Color);
			}

		$this->selectFont(PHPLIB_DIR."pdf/Helvetica.afm");
		$this->setColor(0,0,0);
	}

	function ChooseHeaders($HeaderCover, $HeaderRest = "")
	{
		$this->HeaderCover = strtolower($HeaderCover);
		if ($HeaderRest != "")
			$this->HeaderRest = strtolower($HeaderRest);
		else
			$this->HeaderRest = $this->HeaderCover;
	}

	function GetHeaderData($UseHeader, $PageNum)
	{
		$Func = trim("HeaderData".ucfirst(strtolower($UseHeader)));
		if ($Func != "HeaderData")
			$this->$Func($PageNum);
	}

	function GetFooterData($UseFooter, $PageNum, $TotalPageNum)
	{
		$this->FooterPageNum = ($PageNum * 1);
		$this->FooterTotalPageNum = ($TotalPageNum * 1);
		$Func = trim("FooterData".ucfirst(strtolower($UseFooter)));
		if ($Func != "FooterData")
			$this->$Func($PageNum);
	}

	function FullLine($Y, $Style = 1, $Dotted = false, $Coloured = false)
	{
		if (!$Coloured)
		{
			$this->setColor(0,0,0,1);
			$this->setStrokeColor(0,0,0,1);
		}
		if ($Dotted)
			$this->setLineStyle($Style, 'round', '', array(10,20));
		else
			$this->setLineStyle($Style);
		$this->line($this->ez['leftMargin'], $Y, ($this->ez['pageWidth'] - $this->ez['rightMargin']), $Y);
		$this->setLineStyle(1);
	}

	function TextAlign($Text, $X, $Y, $Size, $Width, $Align, $Color = array())
	{
		$Size = $Size * 1;
		if (!$Size)
			exit("<B>ERROR: Font size unspecified for the following text:</B><P>".$Text);
		if (!$Width)
			$Width = $this->ez['pageWidth'] - $this->ez['rightMargin'] - $X;
		while ($Text)
		{
			$Text = $this->addTextWrap($X, $Y, $Width, $Size, $Text, $Align, 0, 0, $Color);
			$Y = round($Y - ($this->getFontHeight($Size) * 1.1));
		}
	}

	function TextLeft($Text, $X, $Y, $Size = 12, $Width = 0)
	{
		$this->TextAlign($Text, $X, $Y, $Size, $Width, "left");
	}

	function TextRight($Text, $X, $Y, $Size = 12, $Width = 0)
	{
		$this->TextAlign($Text, $X, $Y, $Size, $Width, "right");
	}

	function TextCenter($Text, $X, $Y, $Size = 12, $Width = 0)
	{
		$this->TextAlign($Text, $X, $Y, $Size, $Width, "center");
	}

	function TextJust($Text, $X, $Y, $Size = 12, $Width = 0)
	{
		$this->TextAlign($Text, $X, $Y, $Size, $Width, "full");
	}

	function PlaceText($Text, $X, $Y, $Size = 12, $Align = "left", $Width = "")
	{
		$Function = "Text".ucfirst(strtolower($Align));
		$Lines = explode("\n", $Text);
		for ($i=0;$i<sizeof($Lines);$i++)
		{
			$Y = round($Y - ($Size) * 1.1);
			$this->$Function($Lines[$i], $X, $Y, $Size, $Width);
		}
		$this->selectFont(PHPLIB_DIR."pdf/Helvetica.afm");
		$this->setColor(0,0,0);
	}

	function NewTable($TableID)
	{
		$this->Table[$TableID] = array();

		$this->Table[$TableID]["Options"]["showLines"]			= 1;
		$this->Table[$TableID]["Options"]["showHeadings"]		= 1;
		$this->Table[$TableID]["Options"]["shaded"]				= 2;
		$this->Table[$TableID]["Options"]["shadeCol"]			= array(0.8,0.8,0.8);
		$this->Table[$TableID]["Options"]["shadeCol2"]			= array(0.7,0.7,0.7);
		$this->Table[$TableID]["Options"]["fontSize"]			= 10;
		$this->Table[$TableID]["Options"]["textCol"]			= array(0,0,0);
		$this->Table[$TableID]["Options"]["titleFontSize"]		= 12;
		$this->Table[$TableID]["Options"]["titleGap"]			= 5;
		$this->Table[$TableID]["Options"]["rowGap"]				= 0;
		$this->Table[$TableID]["Options"]["colGap"]				= 1.5;
		$this->Table[$TableID]["Options"]["lineCol"]			= array(0,0,0);
		$this->Table[$TableID]["Options"]["innerLineThickNess"]	= 1;
		$this->Table[$TableID]["Options"]["outerLineThickNess"]	= 1;
	//	$this->Table[$TableID]["Options"]["xPos"]				= "center";
	//	$this->Table[$TableID]["Options"]["xOrientation"]		= "center";
	//	$this->Table[$TableID]["Options"]["width"]				= ;
		$this->Table[$TableID]["Options"]["maxWidth"]			= $this->ez["pageWidth"] - $this->ez["leftMargin"] - $this->ez["rightMargin"];
		$this->Table[$TableID]["Options"]["cols"]				= array();

		$this->Table[$TableID]["Columns"]						= array();
		$this->Table[$TableID]["Data"]							= array();
		$this->Table[$TableID]["Title"]							= "";
	}

	function TableOption($TableID, $Option, $Value)
	{
		$this->Table[$TableID]["Options"][$Option]	= $Value;
	}

	function TableAddColumn($TableID, $ColumnName, $Alignment = "left", $Width = 0)
	{
		$Tmp = array();
		$Tmp['justification'] = $Alignment;
		if ($Width)
			$Tmp['width'] = $Width;
		$i = sizeof($this->Table[$TableID]["Columns"]);
		$this->Table[$TableID]["Columns"]["Col".$i] = $ColumnName;
		$this->Table[$TableID]["Options"]["cols"]["Col".$i] = $Tmp;
	}

	function TableAddDataRow($TableID, $DataArray, $DataTextColor = array(), $DataShadeColor = array())
	{
		$TempArray = array();
		for($i=0;$i<sizeof($DataArray);$i++)
		{
			$TempArray["Col".$i] = $DataArray[$i];
			if(is_array($DataTextColor) && isset($DataTextColor[$i]) && count($DataTextColor[$i]))
			{
				$TempArray["TextColor".$i] = $DataTextColor[$i];
			}
			if(is_array($DataShadeColor) && isset($DataShadeColor[$i]) && count($DataShadeColor[$i]))
			{
				$TempArray["ShadeColor".$i] = $DataShadeColor[$i];
			}
		}
		$this->Table[$TableID]["Data"][] = $TempArray;
	}

	function TableAddTitle($TableID, $TableTitle)
	{
		$this->Table[$TableID]["Title"] = $TableTitle;
	}

	function PublishTable($TableID)
	{
		$this->ezTable($this->Table[$TableID]["Data"],
					   $this->Table[$TableID]["Columns"],
					   $this->Table[$TableID]["Title"],
					   $this->Table[$TableID]["Options"]
					  );
		$this->ResetObjectProperties();
	}

	function ChangeError($Message)
	{
		exit ("Error: ".$Message);
	}

	function ChangeTitle($Level, $Attribute, $Value)
	{
		$i = sizeof($this->Contents);

		if ($i == 0)
		{
			if (!in_array($Level, array(1,2,3,4)))
				exit ("Error: Invalid Title Level supplied to 'ChangeTitle'.");

			switch ($Attribute)
			{
				case "font":
					if (in_array($Value, array("Helvetica", "Courier", "Times-Roman")))
						$this->TitleStyle[$Level][$Attribute] = $Value;
					break;
				case "colour":
					if (is_array($Value) && (sizeof($Value) == 3))
						$this->TitleStyle[$Level][$Attribute] = $Value;
					break;
				case "align":
					if (in_array($Value, array("left", "right", "center", "centre", "full")))
						$this->TitleStyle[$Level][$Attribute] = $Value;
					break;
				case "size":
					$Value = $Value * 1;
					if ($Value)
						$this->TitleStyle[$Level][$Attribute] = $Value;
					break;
				case "above":
				case "below":
				case "uppercase":
				case "newpage":
				case "tabs":
				case "italic":
				case "bold":
					$Value = $Value * 1;
					$this->TitleStyle[$Level][$Attribute] = $Value;
					break;
				default:
					break;
			}
		}
	}

	function TitleIndexLevel($Level)
	{
		$Level = $Level * 1;
		if (in_array($Level, array(0, 1, 2, 3, 4)))
			$this->TitleIndexLevel = $Level;
	}

	function DisplayTitle($Text, $Level = 1, $Hide = 0, $Identifier = "")
	{
		$TitlePrefix = "";
		if ($Level <= $this->TitleIndexLevel)
		{
			switch($Level)
			{
				case 1:
					$this->TitleIndexes[1]++;
					$this->TitleIndexes[2] = 0;
					$this->TitleIndexes[3] = 0;
					$this->TitleIndexes[4] = 0;
					$TitlePrefix = $this->TitleIndexes[1].". ";
					break;
				case 2:
					if (!$this->TitleIndexes[1])
						$this->TitleIndexes[1]++;
					$this->TitleIndexes[2]++;
					$this->TitleIndexes[3] = 0;
					$this->TitleIndexes[4] = 0;
					$TitlePrefix = $this->TitleIndexes[1].".".$this->TitleIndexes[2].". ";
					break;
				case 3:
					if (!$this->TitleIndexes[1])
						$this->TitleIndexes[1]++;
					if (!$this->TitleIndexes[2])
						$this->TitleIndexes[2]++;
					$this->TitleIndexes[3]++;
					$this->TitleIndexes[4] = 0;
					$TitlePrefix = $this->TitleIndexes[1].".".$this->TitleIndexes[2].".".$this->TitleIndexes[3].". ";
					break;
				case 4:
					if (!$this->TitleIndexes[1])
						$this->TitleIndexes[1]++;
					if (!$this->TitleIndexes[2])
						$this->TitleIndexes[2]++;
					if (!$this->TitleIndexes[3])
						$this->TitleIndexes[3]++;
					$this->TitleIndexes[4]++;
					$TitlePrefix = $this->TitleIndexes[1].".".$this->TitleIndexes[2].".".$this->TitleIndexes[3].".".$this->TitleIndexes[4].". ";
					break;
			}
		}
		$Text = $TitlePrefix.$Text;

		$i = sizeof($this->Contents);

		if (!in_array($Level, array(1,2,3,4)))
			$Level = 1;

		if ($this->TitleStyle[$Level]["uppercase"])
			$Text = strtoupper($Text);

		if ($this->TitleStyle[$Level]["italic"])
			$Text = "<i>".$Text."</i>";

		if ($this->TitleStyle[$Level]["bold"])
			$Text = "<b>".$Text."</b>";

		if ($this->TitleStyle[$Level]["newpage"] && $i)
			$this->ezNewPage();

		$this->setColor($this->TitleStyle[$Level]["colour"][0], $this->TitleStyle[$Level]["colour"][1], $this->TitleStyle[$Level]["colour"][2]);
		$this->ChangeFont($this->TitleStyle[$Level]["font"]);
		$Options = array();
		$Options["justification"] = $this->TitleStyle[$Level]["align"];
		if ($this->TitleStyle[$Level]["tabs"])
			$Options["left"] = $this->mm($this->TitleStyle[$Level]["tabs"] * 10);
		$Size = $this->TitleStyle[$Level]["size"];

		$Rows = 9 - $Level - $Level;
		if (($this->y - $this->ez["bottomMargin"]) <= ($this->getFontHeight($Size) * $Rows))
			$this->ezNewPage();
		elseif ($this->TitleStyle[$Level]["above"])
			$this->ezSetDy(-round($Size * $this->TitleStyle[$Level]["above"]));

		if (trim($Identifier))
		{
			$Found = 0;
			foreach ($this->Contents as $Row)
				if ($Row["Identifier"] == $Identifier)
					$Found++;
			if ($Found)
				$Identifier = "";
		}
		if (!$Identifier)
			$Identifier = "A".$i;

		$this->addDestination($Identifier, "FitH", $this->y);
		$this->ezText($Text, $Size, $Options);
// $this->ezText($Text." | ".$Identifier, $Size, $Options);
		$this->Contents[$i] = array();
		$this->Contents[$i]["Hide"] = $Hide;
		$this->Contents[$i]["Text"] = $Text;
		$this->Contents[$i]["Level"] = $Level;
		$this->Contents[$i]["Page"] = sizeof($this->ezPages);
		$this->Contents[$i]["Identifier"] = $Identifier;

		if ($this->TitleStyle[$Level]["below"])
			$this->ezSetDy(-round($Size * $this->TitleStyle[$Level]["below"]));
	}

	function DisplayParagraph($Text, $Tabs = 0)
	{
		$this->setColor(0,0,0);
		$this->ChangeFont('Times-Roman');
		$this->ezText($Text."\n", 12, array("justification" => "full", "left" => $this->mm($Tabs * 10)));
	}

	function DisplayQuote($Text)
	{
		$this->DisplayParagraph($Text, 1);
	}

	function DisplayCode($Text, $Tabs = 0)
	{
		$this->setColor(0,0,0);
		$this->ChangeFont('Courier');
		$this->ezText($Text."\n", 12, array("justification" => "full", "left" => $this->mm($Tabs * 10)));
	}

	function DisplayQuoteCode($Text)
	{
		$this->DisplayCode($Text, 1);
	}

	function DisplayContents($Page, $Level = 1, $Text = "Contents", $Color = array(), $GapSize = 0, $FontSize = 12)
	{
		// create a new page and get the object id
		$PageObjectID = $this->newPage(1, $this->ezPages[$Page], "before");
		$this->ezPageInsert($Page, $PageObjectID);

		// add a title to the contents page
		if ($Text)
		{
			$this->setColor(0,0,0);
			$this->ChangeFont('Helvetica');
			$this->TextCenter("<b>".$Text."</b>", $this->ez["leftMargin"], ($this->ez["pageHeight"] - $this->ez["topMargin"] - 30), $this->TitleStyle[$Level]["size"], ($this->ez["pageWidth"] - $this->ez["leftMargin"] - $this->ez["rightMargin"]));
		}

		// change the font to Helvetica
		$this->ChangeFont("Helvetica");

		// array containing a list of the page numbers
		$PageNumbers = array();

		// how many contents pages we have so far
		$ContentsPages = 1;

		// this is how many lines we have put on the page so far
		$c = 0;

		// this is our font height
		$S = $FontSize;

		// the size of the writeable area on the page
		$Y = $this->ez["pageHeight"] - $this->ez["topMargin"] - 60 - $GapSize;

		// the size of the font in PDF units
		$L = round($this->getFontHeight($S) * 1.1);

		// location of the left/right (1/2) margins of the page
		$X1 = $this->ez['leftMargin'];
		$X2 = $this->ez['pageWidth'] - $this->ez['rightMargin'];

		$p = 0;
		// for each item in the Contents array:
		for ($i=0;$i<sizeof($this->Contents);$i++)
		{
			// if this item is on a low enough level to be displayed:
			if (($this->Contents[$i]["Level"] <= $Level) && !$this->Contents[$i]["Hide"])
			{
				// calculate the Y coordinate: page size - how many lines we have done so far - size of the font
				$thisY = $Y - ($c * $L);

				// if our Y coordinate is under the bottom margin:
				if ($thisY < $this->ez["bottomMargin"])
				{
					$Page++;

					// insert a new page and get the object id
					$PageObjectID = $this->newPage(1, $this->ezPages[$Page], "before");
					$this->ezPageInsert($Page, $PageObjectID);

					// increment the number of contents pages
					$ContentsPages++;

					// set the number of lines on the page back to 0
					$c = 0;

					// get our new Y coordinate
					$thisY = $Y - ($c * $L);
				}

				// get the top/bottom (1/2)coordinates of our next line
				$Y1 = $thisY - 3;
				$Y2 = $Y1 + $L;

				// add the clickable content link
				$this->addInternalLink($this->Contents[$i]["Identifier"], $X1, $Y1, $X2, $Y2);

				// get the X (left) coordinate of the text - increases with each level
				$textX = $this->ez["leftMargin"] + (($this->Contents[$i]["Level"] - 1) * $this->mm(5));

				// show the text of the item
				$this->addText($textX, $thisY, $S, $this->Contents[$i]["Text"], 0,0,$Color);

				// store the page number and line in an array
				$PageNumbers[$p]["ObjectID"] = $PageObjectID;
				$PageNumbers[$p]["LineX"] = $textX + $this->getTextWidth($S, $this->Contents[$i]["Text"]) + 10;
				$PageNumbers[$p]["Y"] = $thisY;
				$PageNumbers[$p]["PageSize"] = $S;
				$PageNumbers[$p]["PageNumber"] = $this->Contents[$i]["Page"];
				$p++;

				// increment the line counter
				$c++;
			}
		}
		$PrevID = 0;
		// insert the page numbers - for each item in the PageNumbers array
		for($i=0; $i<sizeof($PageNumbers); $i++)
		{
			// to make it easier to read
			$ThisArray = $PageNumbers[$i];

			// if we have a different page object than the previous one
			if($ThisArray["ObjectID"] <> $PrevID)
			{
				// if there was a previous object id, close it
				if($PrevID)
					$this->closeObject();

				// re open the new page object
				$this->reopenObject($ThisArray["ObjectID"]);
			}

			// increase the page number to what it should be
			$ThisArray["PageNumber"] += $ContentsPages;

			// get the X coordinate for the page number
			$PageX = $this->ez["pageWidth"] - $this->ez["rightMargin"] - $this->getTextWidth($S, trim($ThisArray["PageNumber"]));

			// add the page number
			$this->addText($PageX, $ThisArray["Y"], $ThisArray["PageSize"], $ThisArray["PageNumber"]);

			// show the dotted line going across
			$this->setLineStyle(1, 'round', '', array(0,10));
			$this->line($ThisArray["LineX"], $ThisArray["Y"], $PageX, $ThisArray["Y"]);

			// exactly what it sounds like
			$this->ResetObjectProperties();

		}
		// close the last object if there was one
		if($PrevID)
			$this->closeObject();
	}

	function ezPageInsert($InsertPoint, $InsertObject)
	{
		$DataArray = $this->ezPages;
		$ArrayCopy = array();
		$i = 1;
		for ($a=1;$a<=sizeof($DataArray);$a++)
		{
			if ($InsertPoint == $i)
			{
				$ArrayCopy[$i] = $InsertObject;
				$i++;
			}
			$ArrayCopy[$i] = $DataArray[$a];
			$i++;
		}
		$this->ezPages = $ArrayCopy;
	}

	function BulletList($Lines, $Size = 12, $Options = array())
	{
		for ($i=0;$i<sizeof($Lines);$i++)
			$this->ezText("<C:Bullet>".$Lines[$i], $Size, $Options);
	}

	function Bullet($Data)
	{
		$D = abs($Data["decender"]);
		$X = $Data["x"] - ($D * 2);
		$Y = $Data["y"] + ($D * 1.5);
		$this->setLineStyle($D);
		$this->setStrokeColor(0,0,0);
		$this->ellipse($X, $Y, 1);
	}
}

?>

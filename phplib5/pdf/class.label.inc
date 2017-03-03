<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

//============================================================
// class.label.inc
//
// class for creating a PDF containing correctly laid-out printable labels
//
// Author: Peter Hodge Feb 5, 2003
//
//============================================================
// HOW TO USE:
//
// 1. INITIALIZE THE CLASS:
//
//		$L = new PDFLabel("example");
//
//		label definitions should be stored in PHPLIB_DIR."pdf/labels/"
//		an example label definition file 'example.inc' is located in PHPLIB_DIR."pdf/labels/"
//		(probably ./labels/)
//
//
// 2. MAKE MODIFICATIONS TO LAYOUT
//
//		$L->ChangeLayout("LabelMargin", 10);
//
//		It is possible to change any of the layout options after specifying a layout file.
//		However, if you are making extensive changes to an existing layout, you should really
//		create a new layout file in PHPLIB_DIR."pdf/labels/".
//
//		In most circumstances this would only be used for changing fonts / font sizes, and possibly label margins
//		$L->ChangeLayout("FontName", "Helvetica");
//		$L->ChangeLayout("FontSize", 10);
//
//		This step is optional.
//
//
// 3. ADD AREAS
//
//		$Left = 8;
//		$Top = 5;
//		$Right = -8;
//		$Bottom = -5;
//		$L->AddArea("WholeLabel", $Left, $Top, $Right, $Bottom);
//
//		You need to define where on the label you wish to put your text using the AddArea() function.
//		In the example above, an area is added which takes up the whole label, with 8mm side margins,
//		and 5mm top/bottom margins. If any of the margin values is positive, it is considered to be
// 		measured from the left/top margin.  If any of the margin values are negative, they are considered
//		to be measured inwards from the right/bottom margins
//		In the next example, three areas will be designated, one at the top-left, one at the bottom-right,
//		and one spanning 80mm across from the absolute left
//
//		$L->AddArea("TopLeft", 5, 5, 25, 25);
//		$L->AddArea("LowerRight", -25, -25, -5, -5);
//		$L->AddArea("Span", 0, 30, 80, 30);
//
//
// 4. ADD LABELS
//
//		$Address = "30 Long St\nBrisbane QLD 4000";
//		$L->AddLabel("NameArea", "John Smith");
//		$L->AddLabel("AddressArea", $Address);
//		$L->NextLabel();
//
//		Make sure you use '\n' for new lines in the label text.
//
//
// 4. SEND THE PDF FILE TO THE BROWSER
//
//		$L->ezStream();
//
//============================================================

require_once(PHPLIB_DIR."/pdf/class.acrobat.inc");




class PDFLabel extends ACROBAT
{
	var $DebugErrors = 0;
	var $Outline = 0;
	var $Areas = array();
	var $Valid = array(
		"Options" => array("Across", "Down", "PageMargins", "Spacing", "PageSize", "FontName", "FontSize"),
		"Just" => array("left", "right", "center", "centre", "full"),
		"VAlign" => array("center", "top", "centre"),
		);
	var $LabelData = array();
	var $Done = array();
	var $MM2PDF = 0;

	function __construct($Layout, $INC = "")
	{
		parent::__construct();

		// set up the default values
		$this->LayoutOptions = array(
			"Across" => 2,
			"Down" => 6,
			"PageMargins" => 0,
			"Spacing" => array(0, 0),
			"PageSize" => "a4 portrait",
			"FontName" => "Helvetica",
			"FontSize" => 10,
			);

		$this->MM2PDF = 72 / 25.4;

		// include the layout file
		if(!$INC)
			$INC = PHPLIB_DIR."pdf/labels/";

		require($INC.$Layout.".inc");
		foreach($LABEL_LAYOUT as $OptionName => $Value)
			$this->LayoutOptions[$OptionName] = $Value;
	}

	function ChangeLayout($OptionName, $Value)
	{
		if($this->DebugErrors)
		{
			if($this->Done["Layout"])
				$this->E("ChangeLayout()", "you can't change the layout after you have defined areas - "
					." <i>".implode(", ", array_keys($this->Areas))."</i>!");

			if(!in_array($OptionName, $this->Valid["Options"]))
				$this->E("ChangeLayout()", "invalid Option Name '".$OptionName."'!"
					." Must be one of: <i>".implode(", ", $this->Valid["Options"])."</i>");
		}

		$this->LayoutOptions[$OptionName] = $Value;
	}

	function PrepareLayout()
	{
		if($this->Done["Layout"])
			return;

		extract($this->LayoutOptions);

		// work out 4 page margins
		if(is_array($PageMargins))
			list($PTop, $PBottom, $PLeft, $PRight) = $PageMargins;
		else
			$PTop = $PBottom = $PLeft = $PRight = $PageMargins;

		// work out page / label sizes
		$PHeight = 295;
		$PWidth = 211;

		// work out horizontal / vertical spacing
		list($HSpacing, $VSpacing) = $Spacing;

		// work out how much horizontal / vertical space is taken by all labels
		$LHeight = $PHeight - ($PTop + $PBottom);
		$LWidth = $PWidth - ($PLeft + $PRight);

		// minus vertical / horizontal spacing
		$LHeight -= $VSpacing * ($Down - 1);
		$LWidth -= $HSpacing * ($Across - 1);

		// divide the height / width by the number of labels to get the size of a real label
		$LHeight /= $Down;
		$LWidth /= $Across;

		$this->Layout = array();

		for($R=0; $R<$Down; $R++)
			for($C=0; $C<$Across; $C++)
			{
				// save these numbers into the array
				$Array =& $this->Layout[];

				// calculate X / Y coordinates
				$Array["Left"] = $PLeft;
				$Array["Left"] += ($LWidth + $HSpacing) * $C;
				$Array["Top"] = $PHeight - $PTop;
				$Array["Top"] -= ($LHeight + $VSpacing) * $R;

				// convert all units to PDF
				$Array["Left"] *= $this->MM2PDF;
				$Array["Top"] *= $this->MM2PDF;
			}

		// the width and height need to be stored as well 
		// convert these two variables to PDF units
		$this->LabelData["Height"] = $LHeight * $this->MM2PDF;
		$this->LabelData["Width"] = $LWidth * $this->MM2PDF;

		list($PaperSize, $PageLayout) = explode(" ", $PageSize);

		// call constructor for ezPDF
		$this->Cezpdf($PaperSize, $PaperSize);

		// make sure the font exists
		$Font = PHPLIB_DIR."pdf/".$FontName.".afm";
		if(!file_exists($Font))
			$this->E("PrepareLayout", "font '$FontName' does not exist in ".$Font);

		$this->selectFont($Font);
		$this->FontSize = $FontSize;

		$this->ezSetMargins($PTop, $PBottom, $PLeft, $PRight);

		$this->LayoutPointer = 0;

		$this->Done["Layout"] = 1;
	}

	function GetLabelWidth()
	{
		$this->PrepareLayout();
		return $this->LabelData["Width"] / $this->MM2PDF;
	}

	function GetLabelHeight()
	{
		$this->PrepareLayout();
		return $this->LabelData["Height"] / $this->MM2PDF;
	}

	function AddArea($Name, $X1, $Y1, $X2, $Y2, $Just = "", $VAlign = "", $FontSize = 0)
	{
		$this->PrepareLayout();

		foreach(array("Just", "VAlign") as $AlignType)
			if(!$$AlignType)
				$$AlignType = $this->Valid[$AlignType][0];

		// check the function arguments
		if($this->DebugErrors)
		{
			if(!$Name)
				$this->E("DefineArea()", "no \$Name provided!"
					." (Existing: <i>".implode(", ", array_keys($this->Areas))."</i>)");

			// see if we are allowed to add more areas
			if($this->Done["Areas"])
				$this->E("DefineArea($Name)", "You can't add more areas once label data has been added!");

			foreach(array("Just", "VAlign") as $AlignType)
				if(!in_array($$AlignType, $this->Valid[$AlignType]))
					$this->E("DefineArea($Name)", "'".$$AlignType."' is an invalid value for \$".$AlignType."!"
						." Must be one of: <i>".implode(", ", $this->Valid[$AlignType])."</i>");

			// make sure coordinates are numeric
			foreach(array("X1", "Y1", "X2", "Y2") as $Var)
				if(!is_numeric($$Var))
					$this->E("DefineArea($Name)", $Var." ('".$$Var."') must be numeric!");

			// make sure the X1/2 and Y1/2 values are not too low or too high
			foreach(array("X"=>"Width", "Y"=>"Height") as $Var => $Max)
			{
				$Var1 = $Var."1";
				$Var2 = $Var."2";
				$Var1Real = $$Var1 + ($$Var1 < 0 ? $this->LabelData[$Max] : 0);
//				$Var2Real = $$Var2 + ($$Var2 < 0 ? $this->LabelData[$Max] : 0);

				if($$Var1 < 0 && $Var1Real < 0)
					$this->E("ChangeLayout($Name)", "$".$Var1." ('".$$Var1."') equates to '".$Var1Real."'"
						.", which must <b>not</b> be less than 0!");

//				if($$Var2 < 0 && $Var2Real <= $$Var1Real)
//					$this->E("ChangeLayout($Name)", "$".$Var2." ('".$$Var2."') equates to '".$Var2Real."'"
//						.", which must be greater than $".$Var1." ('".$$Var1Real."')");
				if($$Var2 < 0)
					$this->E("ChangeLayout($Name)", "$".$Var2." ('".$$Var2."') must not be less than 0");
			}

			if($FontSize && !is_numeric($FontSize))
				$this->E("DefineArea($Name)", "\$FontSize must be numeric!");
			else if($FontSize < 0)
				$this->E("DefineArea($Name)", "\$FontSize cannot be less than 0!");
		}

		// convert X2 / Y2 to positive if they are negative (from right/bottom margin)
		if($X1 < 0)
			$X1 += $this->GetLabelWidth();
		if($Y1 < 0)
			$Y1 += $this->GetLabelHeight();

//		if($X2 <= 0)
//			$X2 += $this->LabelData["Width"];
		$X2 = $this->GetLabelWidth() - $X2;
//		if($Y2 <= 0)
//			$Y2 += $this->LabelData["Height"];
		$Y2 = $this->GetLabelHeight() - $Y2;
		
		if($this->DebugErrors)
		{
			// make sure the X1/2 and Y1/2 values are not too low or too high
			foreach(array("X"=>"Width", "Y"=>"Height") as $Var => $Max)
			{
				$Var1 = $Var."1";
				$Var2 = $Var."2";

				if($$Var1 < 0)
					$this->E("ChangeLayout($Name)", "$".$Var1." ('".$$Var1."') must not be less than 0!");

				if($$Var1 >= $this->LabelData[$Max])
					$this->E("ChangeLayout($Name)", "$".$Var1." ('".$$Var1."') must be less than"
						." Label ".$Max.": '".$this->LabelData[$Max]."'!");

				if($$Var2 >= 0 && $$Var2 <= $$Var1)
					$this->E("ChangeLayout($Name)", "$".$Var2." ('".$$Var2."') must be greater than"
						." $".$Var1." ('".$$Var1."')");

				if($$Var1 < 0 && $Var1Real < 0)
					$this->E("ChangeLayout($Name)", "$".$Var1." ('".$$Var1."') equates to '".$Var1Real."'"
						.", which must not be less than 0!");

				if($$Var2 < 0 && $Var2Real <= $$Var1Real)
					$this->E("ChangeLayout($Name)", "$".$Var2." ('".$$Var2."') equates to '".$Var2Real."'"
						.", which must be greater than $".$Var1." ('".$$Var1."')");
			}
		}

		// convert X1, X2, Y1, Y2 to PDF units
		foreach(array("X1", "Y1", "X2", "Y2") as $Var)
			$$Var *= $this->MM2PDF;

		if($this->DebugErrors)
		{
			// make sure this area does not overlap another
			foreach($this->Areas as $OtherName => $Other)
			{
				// this is rather complicated logic, but it works
				if(
					// left side is not past the right side of the other area, and vice versa
					$X1 < $Other["X2"] && $Other["X1"] < $X2
					// top edge is not past the bottom edge of the other area, and vice versa
					&& $Y1 < $Other["Y2"] && $Other["Y1"] < $Y2
					)
					$this->E("DefineArea($Name)", "This area cannot be added as it overlaps area <i>".$OtherName."</i>!");
			}
		}

		// put everything in this array
		$Area =& $this->Areas[$Name];

		foreach(array("X1", "Y1", "X2", "Y2", "Just", "VAlign") as $Var)
			$Area[$Var] = $$Var;

		$Area["FontSize"] = ($FontSize ? $FontSize : $this->FontSize);

		$Area["Height"] = $Y2 - $Y1;
		$Area["Width"] = $X2 - $X1;
	}

	function AddData($AreaName, $LabelText)
	{
		$this->TestArea("AddData", $AreaName, $LabelText);

		// if we are passed the end of Layout, start a new page and go back to the beginning
		if($this->LayoutPointer == sizeof($this->Layout))
		{
			$this->ezNewPage();
			$this->LayoutPointer = 0;
		}

		// if there is no text, there's no use wasting time writing it to the PDF!
		if(!$LabelText)
			return;

		// find the edge coordinates for this label
		extract($this->GetAreaCoords($AreaName)); // $Top, $Bottom, $Left, $Right, $Just, $VAlign, $Height, $Width

		// show an outline around this area
		if($this->Outline)
			$this->rectangle($Left, $Top - $Height, $Width, $Height);

		// set up area limitations and alignment
		$TextOptions = array("aleft"=>$Left, "aright"=>$Right, "justification"=>$Just);

		// if we are valigning centered, we'll need to use a transaction
		if($VAlign == "center" || $VAlign == "centre")
		{
			// start a transaction and write the label contents
			$this->transaction("start");
			$this->ezSetY($Top);
			$this->ezText($LabelText, $FontSize, $TextOptions);

			// get the Y coord where it finished and get the new Y coord for centering
			$EndY = $this->y;
			$SpaceNeeded = $Top - $EndY;
			$NewTop = $Top - (($Height - $SpaceNeeded) / 2);

			// cancel the transaction and write the label in the new Y coord
			$this->transaction("abort");
			$Top = $NewTop;
		}

		$this->ezSetY($Top);
		$this->ezText($LabelText, $FontSize, $TextOptions);
	}

	function AddJPGWeb($AreaName, $URL, $ImageWidth, $ImageHeight)
	{
		$this->TestArea("AddJPGWeb", $AreaName, $URL, $ImageWidth, $ImageHeight);

//		if(!$Width)
//			$Width = $this->Areas[$AreaName]["Width"];
//		if(!$Height)
//			$Height = $this->Areas[$AreaName]["Height"];

		// find the edge coordinates for this label
		extract($this->GetAreaCoords($AreaName)); // $Top, $Bottom, $Left, $Right, $Just, $VAlign, $Height, $Width

		if($this->DebugErrors)
		{
			$ImageSize = getimagesize($URL);

			// check that the image exists
			if(!$ImageSize)
				$this->E(__FUNCTION__."($AreaName)", "Image '".$URL."' does not exist!");

			// extract these values from the $ImageSize array
			list($RealWidth, $RealHeight) = $ImageSize;

			foreach(array("Width", "Height") as $Dim)
			{
				$ImageDim = "Image".$Dim;

				// if there is no width/height provided, try getting it from the image size data
				if(!$$ImageDim)
					continue;

				// make sure the dimension is numeric
				if(!is_numeric($$ImageDim))
					$this->E(__FUNCTION__."($AreaName, $URL)",
						"Image ".$Dim." ('".$$ImageDim."') must be numeric!");

				// make sure the dimension isn't less than 0
				if($$ImageDim < 0)
					$this->E(__FUNCTION__."($AreaName, $URL)",
						"Image ".$Dim." ('".$$ImageDim."') cannot have a negative value!");

				// make sure user didn't specify dimensions that are too big
				if($$ImageDim > $$Dim)
					$this->E(__FUNCTION__."($AreaName, $URL)",
						"Image ".$Dim." ('".$$ImageDim."') cannot be greater than"
							." Area <i>".$AreaName."</i> ".$Dim." ('".$$Dim."')");
			}
		}

		// if we are passed the end of Layout, start a new page and go back to the beginning
		if($this->LayoutPointer == sizeof($this->Layout))
		{
			$this->ezNewPage();
			$this->LayoutPointer = 0;
		}

		// show an outline around this area
		if($this->Outline)
			$this->rectangle($Left, $Top - $Height, $Width, $Height);

		$this->addGIFFromWeb($URL, $Bottom, $Left, $ImageWidth, $ImageHeight);
	}

	function GetAreaCoords($AreaName)
	{
		// if we are passed the end of Layout, start a new page and go back to the beginning
		if($this->LayoutPointer == sizeof($this->Layout))
		{
			$this->ezNewPage();
			$this->LayoutPointer = 0;
		}

		$Layout =& $this->Layout[$this->LayoutPointer];
		$Area =& $this->Areas[$AreaName];

		$Return = array(
			"Top" => $Layout["Top"] - $Area["Y1"],
			"Bottom" => $Layout["Top"] - $Area["Y2"],
			"Left" => $Layout["Left"] + $Area["X1"],
			"Right" => $Layout["Left"] + $Area["X2"],
			"Just" => $Area["Just"],
			"VAlign" => $Area["VAlign"],
			"Height" => $Area["Height"],
			"Width" => $Area["Width"],
			"FontSize" => $Area["FontSize"],
			);
		return $Return;
	}

	function NextLabel()
	{
		// outline the current label if we're supposed to
		if($this->Outline)
		{
			extract($this->Layout[$this->LayoutPointer]); // $Top, $Left
			extract($this->LabelData); // $Width, $Height

			$this->rectangle($Left, $Top - $Height, $Width, $Height);
		}

		if($this->DebugErrors)
			$this->Done["Areas"] = array();

		$this->LayoutPointer++;
	}

	// for the purpose of debugging:
	function SetDebugErrors()
	{
		$this->DebugErrors = 1;
	}

	function SetAreaOutline()
	{
		$this->Outline = 1;
	}

	function E($Func, $Msg)
	{
		trigger_error("class PDFLabel -> ".$Func.":<BR>".$Msg."<BR>", E_USER_ERROR);
	}

	function TestArea($Func, $AreaName)
	{
		if(!$this->DebugErrors)
			return;

		// make sure we have areas!
		if(!sizeof($this->Areas))
			$this->E($Func."()", "You must add at least one area first!");

		// make sure we have an area name
		if(!$AreaName)
			$this->E($Func."()", "You must provide an Area Name!");

		// make sure the area name exists
		if(!$this->Areas[$AreaName])
			$this->E($Func."($AreaName)", "Area '".$AreaName."' was never added!"
				." Existing areas: <i>".implode(", ", array_keys($this->Areas))."</i>");

		// make sure the area isn't taken already ... mark it as done
		$Done =& $this->Done["Areas"][$AreaName];
		if($Done)
			$this->E($Func."($AreaName)", "Area '".$AreaName."' is already used by '".$Done."'");
		$Done = $LabelText ? $LabelText : "(Empty String)";
	}
}

?>

<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

require_once("definitions.inc");
require_once(ToIncludes("class")."course.inc");

$Now = date("Y-m-d");

// Find all courses where they start in the future
$Qry = "SELECT * FROM course ";
$Qry .= "WHERE start_date > now()";

$Q = new DBQuery;
$Q->Run(FFS_DB, $Qry);
while($Q->Next())
{
	$CourseID = $Q->GetField("course_id");
	$Course = new course($CourseID);
	$CourseData = $Course->GetData();
	$CourseCost = $CourseData["cost"];

// Check to see if today is the cutoff date
	$CutoffDays = $CourseData["paid_by_timeframe"];
	$StartDate = $CourseData["start_date"];
	$CutoffDate = date("Y-m-d", strtotime("$StartDate - $CutoffDays DAYS"));
	if($Now != $CutoffDate)
	{
		continue;
	}
	
// Grab the enrolments for the course
	$Qry2 = "SELECT * FROM enrolment WHERE course_id = $CourseID";
	$Q2 = new DBQuery;
	$Q2->Run(FFS_DB, $Qry2);
	while($Q2->Next())
	{
		$EnrolmentID = $Q2->GetField("enrolment_id");
		$Qry3 = "SELECT sum(amount) AS total_paid FROM payments WHERE enrolment_id = $EnrolmentID";
		$Q3 = new DBQuery;
		$Q3->RunOne(FFS_DB, $Qry3);
		$AmountPaid = $Q3->GetField("total_paid");
		if($AmountPaid < $CourseCost)
		{
			$UnpaidAmount = $CourseCost - $AmountPaid;
			$ChildID = $Q2->GetField("child_id");
			$UserID = $Q2->GetField("jos_user_id");
			EmailCoursePaymentReminder($EnrolmentID, $UnpaidAmount, $CourseID, $StartDate, $ChildID, $UserID, $CourseCost);
		}

	}
}


function EmailCoursePaymentReminder($EnrolmentID, $UnpaidAmount, $CourseID, $CourseDate, $ChildID, $UserID, $CourseCost)
{
	// Email user and provide link to payment
	// http://cmffs.webstract.com.au/index.php?Action=class|payment&EnrolmentID=$EnrolmentID
	echo "Would Email for $EnrolmentID, $UnpaidAmount, $CourseID, $CourseDate, $ChildID, $UserID, $CourseCost\n";
}
?>

<?php
// mailq.php - mail q sender processor
// runs: every minute

require_once("definitions.inc");
require_once(PHPLIB_DIR."mailq.inc");

define(MAIL_Q_RECIPIENT, "\"Murray Shields\" <murray@shields.id.au>");

$Message = "This email is in test mode and will only be delivered to Murray Shields.";

$Message .= "\n\nThis message was created at ".date("Y-m-d H:i:s")."\n";

$M = new MailQ();
$M->AddEmail("Test email ".$argv[1],
	$Message,
	"murray@shields.id.au");
$M->AddRecipients("murray@shields.id.au");
//$M->AddDocument(file_get_contents("/home/peterh/Desktop/browser.pdf"), "application/pdf", "myattachment.pdf", 1);
//$M->AddDocument(file_get_contents("/home/peterh/supras/mud.jpg"), "image/jpeg", "supra.jpg", 0);
//$M->DeliveryAfter("2004-11-04 09:50:00");
//$M->DelayDeliveryBy("+7 minutes");
$Error = $M->Send();
if($Error)
	echo "ERROR: ".$Error."\n";

?>

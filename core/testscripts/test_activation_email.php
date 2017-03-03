<?php

require_once("../bin/definitions.inc");
require_once(ToIncludes("core")."user_security.inc");

$User = new user_security(95);

$GLOBALS["COMPANY_NAME"] = "Flying Fox";
$GLOBALS["COMPANY_PHONE"] = "234234";
$GLOBALS["COMPANY_FAX"] = "234234";
$GLOBALS["FROM_EMAIL"] = "craig@webstract.com.au";
$_SERVER["HTTP_HOST"] = "cmffs.webstract.com.au";

$User->SendActivationEmail(array("email" => "rod@thecoles.id.au"), "ACTIVATION");



?>

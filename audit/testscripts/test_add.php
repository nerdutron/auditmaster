<?php

require_once("definitions.inc");
require_once(ToIncludes("core")."user_security.inc");
require_once(ToIncludes("audit")."operator.inc");
require_once(ToIncludes("audit")."audit.inc");
require_once(ToIncludes("audit")."scheme.inc");
require_once(ToIncludes("audit")."audit_business.inc");


$User = new user_security(3);

$AB = new audit_business($User, 4066);

$AB->SchemeSignup(5);


?>

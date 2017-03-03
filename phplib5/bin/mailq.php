<?php
// mailq.php - mail q sender processor
// runs: every minute

require_once("definitions.inc");
require_once(PHPLIB_DIR."mailq.inc");

MailQ::Process();

?>

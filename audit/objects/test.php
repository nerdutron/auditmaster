<?php

echo mime_content_type("uploads/invoiceCSC2639.pdf");


$finfo = new finfo(FILEINFO_MIME, "/usr/share/misc/magic"); // return mime type ala mimetype extension

if (!$finfo) {
    echo "Opening fileinfo database failed";
    exit();
}

/* get mime-type for a specific file */
$filename = "/usr/local/something.txt";
echo $finfo->file($filename);

?>

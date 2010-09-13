<?php
include_once 'Channels/Channels.inc';
include_once 'DAL/DAL.inc';

Channels::includeSystem('Help');

Help::createDocs();

?>

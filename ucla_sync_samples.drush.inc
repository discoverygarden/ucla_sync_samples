<?php

function ucla_sync_samples_drush_command() {
  $items['ucla_sync_ingest'] = array(
    'callback' => 'ucla_sync_ingest',
    'description' => dt('Ingest Sample Data'),
    'examples' => array(
      'drush -u 1 -l http://example.com ucla_sync_ingest'
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_LOGIN,
  );
  return $items;
}

function ucla_sync_ingest() {
  module_load_include('inc', 'islandora', 'includes/utilities');
  for ($i = 100; $i <= 150; $i++) {
    $mods_url = "http://digital.library.ucla.edu/islandora/object/edu.ucla.library.specialCollections.losAngelesDailyNews:$i/datastream/MODS";
    $mods = file_get_contents($mods_url);
    $mods_xml_doc = new DOMDocument();
    $mods_xml_doc->loadXML($mods);

    $mods_title_xpath = new DOMXpath($mods_xml_doc);
    $mods_title = $mods_title_xpath->query('/mods:mods/mods:titleInfo/mods:title');

    $title = $mods_title->item(0)->nodeValue;

    $jpeg_url = "http://digital.library.ucla.edu/islandora/object/edu.ucla.library.specialCollections.losAngelesDailyNews:$i/datastream/JPG/view";
    $jpeg = file_get_contents($jpeg_url);

    $object = islandora_prepare_new_object('moo', $title, array(), array('islandora:sp_basic_image'), array(array('relationship' => 'isMemberOfCollection', 'pid' => 'islandora:sp_basic_image_collection')));

    $mods_ds = $object->constructDatastream('MODS', 'M');
    $mods_ds->label = 'MODS Record';
    $mods_ds->mimeType = 'text/xml';
    $mods_ds->setContentFromString($mods);
    $object->ingestDatastream($mods_ds);

    $obj_ds = $object->constructDatastream('OBJ', 'M');
    $obj_ds->label = "OBJ";
    $obj_ds->mimeType = 'image/jpeg';
    $obj_ds->setContentFromString($jpeg);
    $object->ingestDatastream($obj_ds);

    islandora_add_object($object);

    drush_print('Ingested object: ' . $object->id);
  }

  drush_print_timers();
}
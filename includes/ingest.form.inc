<?php

/**
 * @file
 * The ingest extra metadata form.
 * allows multiple XML metadata forms in the ingest step of Islandora
 * Diego Pino Navarro, OCT 2014
 */

/**
 * The ingest form.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 * @param array $association
 *   The form association.
 *
 * @return array
 *   A drupal form definition.
 */
function islandora_more_metadata_xml_forms_additional_ingest_form(array $form, array &$form_state, array $association) {
  form_load_include($form_state, 'inc', 'islandora_more_metadata_xml_forms', 'ingest.form'); //Not needed, main ingest.inc is/should already adding this
  $step_storage = &islandora_ingest_form_get_step_storage($form_state, islandora_ingest_form_get_current_step_id($form_state));
  $step_storage['association'] = $association;
  $form_name = $association['form_name'];
  $dsid = $association['dsid'];
  $object = islandora_ingest_form_get_object($form_state);
  $template = is_string($association['template']) && !empty($association['template']) ? $association['template'] : NULL;
  $xml = isset($object[$dsid]) ? $object[$dsid]->content : $template;
  unset($form_state['storage'][FormStorage::STORAGE_ROOT]);
   $form = xml_form_builder_get_form($form, $form_state, $form_name, $xml);
  return $form;
}

/**
 * Alter the form such that previous will unset any stored form.
 */
function islandora_more_metadata_xml_forms_additional_ingest_form_alter(array &$form, array &$form_state, $form_id) {
  if (isset($form['prev']['#submit'])) {
    array_unshift($form['prev']['#submit'], 'islandora_more_metadata_xml_forms_additional_ingest_form_previous_submit');
  }
}

/**
 * Unset the form storage.
 */

function islandora_more_metadata_xml_forms_ingest_form_previous_submit(array $form, array &$form_state) {
  unset($form_state['storage'][FormStorage::STORAGE_ROOT]);
  $step_storage = &islandora_ingest_form_get_step_storage($form_state, islandora_ingest_form_get_current_step_id($form_state));
}

/**
 * Updates the ingestable object's datastream.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_more_metadata_xml_forms_additional_ingest_form_submit(array $form, array &$form_state) {
  $step_storage = &islandora_ingest_form_get_step_storage($form_state, islandora_ingest_form_get_current_step_id($form_state));
  $association = $step_storage['association'];
  $object = islandora_ingest_form_get_object($form_state);
  $xml_form = new XMLForm($form_state);
  $document = $xml_form->submit($form, $form_state);
  $document = $document->document;
  //we are not going to apply transforms, neither set object->label, because this is not a primary metadata form, just an ingest step that uses XML forms. Diego Pino
  $created = xml_form_builder_update_metadata_datastream($object, $association['dsid'], $document);
  $step_storage['created']= array($association['dsid'] => $created);
}
 
/**
 * Undoes the submit, purging any datastreams created by this step.
 *
 * @param array $form
 *   The Drupal form.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_more_metadata_xml_forms_additional_ingest_form_undo_submit(array $form, array &$form_state) {
  $step_storage = &islandora_ingest_form_get_step_storage($form_state, islandora_ingest_form_get_current_step_id($form_state));
  $object = islandora_ingest_form_get_object($form_state);
  $association = $step_storage['association'];
  foreach ($step_storage['created'] as $dsid => $created) {
    if ($created) {
      $object->purgeDatastream($dsid);
    }
  }
  unset($step_storage['created']);
  unset($step_storage['association']);
}







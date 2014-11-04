<?php
/**
 * Updates the datastream's content with generated xml from a metadata step form (not the primary)
 *
 * Does not update objects label nor makes XLST transforms. It only replaces the edit metadataform in case we are defining a metadata step in our ingest steps
 * Diego Pino N, Oct 2014
 * @param array $form
 *   The drupal form.
 * @param array $form_state
 *   The drupal form state.
 */
 
function islandora_more_metadata_xml_forms_edit_datastream_form_metadata_form_submit(array $form, array &$form_state) {
  $datastream = $form_state['datastream'];
  $association = $form_state['association'];
  $object = islandora_object_load($datastream->parent->id);
  $xml_form = new XMLForm($form_state);
  $document = $xml_form->submit($form, $form_state);
  $document = $document->document;
  $updated = xml_form_builder_update_metadata_datastream($object, $association['dsid'], $document);
  $form_state['redirect'] = "islandora/object/{$object->id}/manage";
}
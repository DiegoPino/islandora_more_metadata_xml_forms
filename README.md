# Islandora_more_metadata_xml_forms
=================================

Islandora Developer add-on Module to allow more than one XML metadata form in a Content Model Ingest Step as defined in a Islandora Solution Pack


## Introduction

This module extends Islandoras XML Forms functionality to allow multiple XML generated Metadata Ingest Forms in a single Ingest Step.

Islandora XML Forms implements drupal hooks that allow a custom solution pack to define, associate and of course use XML forms for defining Metadata ingest forms for Fedora Commons Objects.

Current implementation only allows one of this associated Metadata forms to be used on Ingest. That means: if a developer associates two different XML forms to an Islandora Content Model, Islandora XML forms adds a "Metadata Form Choice" step, and on Ingest, the user has to choose which one to use. But sometimes, a particular Content Model can have multiple Datastreams that could/should be user-filled in consecutive ingest steps using XML forms.

Islandora on other hand allows to define Multiple Ingest steps that are added as additional forms or callbacks during ingest. But if you choose to add Forms, they need to be defined by code (drupal forms) on a case-by-case scenario.

This Module addresses this need by allowing to use one or more associated XML forms as consecutive Ingest Steps. 

Additional, this module also modifies Form behavior on datastream editing. All XML forms that are used in custom ingest steps, using this module, have their "Object Title updating" and "to DC transforms" disabled. That is nice, because you don't want that every edit messes with your Object. 


This module requires the following modules/libraries (if you are already using Islandora then i'm sure they are allready installed):

* [Islandora](https://github.com/islandora/islandora)
* [Islandora XML Forms](https://github.com/Islandora/islandora_xml_forms)

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Configuration

There is no configuration to activate this Module. By default this module leaves the current implementation of Islandora XML Forms untouched. That means: if you associate multiple XML forms to an CMODEL, the you get a Choice Form (step) and you can choose which one to use.

## How To use in a Solution Pack
1. Define your new XML forms as usual using 

	```php
	hook_islandora_xml_form_builder_forms()	
	```
	
2. Associate your defined XML forms to a Content Model and a DSID (datstream ID) using:
  
	```php
	hook_islandora_content_model_forms_form_associations() //deprecated!
	```
  
  or
  
  ```
  hook_islandora_xml_form_builder_forms().
  ```

  > And remember: At least one of this forms will act as primary metadata ingest step. This one will be responsible for generating the XSLT transform to DC and setting the objects title. **This form should NOT be used as an additional XML form ingest step**.

3. Now the important part: in your ingest steps hook define you extra XML forms using    ```hook_CMODEL_PID_islandora_ingest_steps().```
  > Note: in this example ("islandora_mymodule"=your module name, "islandora_customCModel"= a defined CMODEL in the form of islandora:customCModel:

	First, decide which already associated forms you want to use and build and 'args' Array() to let this module use them. You have at least two ways of doing this:
	
  * Writing and 'args' Array() by hand for every XML form you wan't use:
    
	```php 	
	$arg=array('id_of_form_as_defined_in_current_module' =>
    'content_model' => 'islandora:customCModel',
	'form_name' => 'Real Form Name', 
	'dsid' => 'secondary_DATASTREAM_ID',
	'template' => FALSE,
	'id'=> 'id_of_form_as_defined_in_current_module',    
	); 
	```
  * Get them using this function call (once for every xml form):
  
	```php 
	xml_form_builder_get_hook_associations($forms, $models, $dsids, $only_enabled));
	```

	
	####Example using (B) option here:
  
	```php
	function islandora_mymodule_islandora_customCModel_islandora_ingest_steps(array $form_state) {  
		$arg1=xml_form_builder_get_hook_associations(array('Real Form Name'), array('islandora:customCModel'), array('a_DATASTREAM_ID'), true);
		$arg2=xml_form_builder_get_hook_associations(array('Real Form Name2'), array('islandora:customCModel'), array('another_DATASTREAM_ID'), true);
		return array(
	    //First extra XML Metadata Form
		'islandora_mymodule_extrametadata1' => array(
				'weight' => 6,
				'type' => 'form',
				'form_id' => 'islandora_more_metadata_xml_forms_additional_ingest_form',
				'module' => 'islandora_more_metadata_xml_forms',
				'file' => 'includes/ingest.form.inc',
				'args' => $arg1,
				),
		//Second extra XML Metadata Form
		'islandora_mymodule_extrametadata2' => array(
					'weight' => 7,
					'type' => 'form',
					'form_id' => 'islandora_more_metadata_xml_forms_additional_ingest_form',
					'module' => 'islandora_more_metadata_xml_forms',
					'file' => 'includes/ingest.form.inc',
					'args' => $arg2,
					),
		//Other step, like and upload form
		'islandora_mymodule_file_upload' => array(
						'weight' => 10,
						'type' => 'form',
						'form_id' => 'islandora_entities_tn_upload_form', //reusing here an upload Form from the great Islandora Entities Module
						'module' => 'islandora_entities',
						'file' => 'includes/tn_upload.form.inc',
						),
					);
	}
	```

4. Debug and test. 
	Quick explanation: If you associate three (3) XML forms to an particular CMODEL and then use one of this in an ingest step, this module will remove (this) the last one from the "XML form Choice Module". If you use two(2) of them, then the "Choice Form" will be fully removed and the one defined in the module, but absent as Ingest step, will be used as primary metadata.

## FAQ
* Q. I installed the module and everything is working just as before.
  * A. Right. The idea behing this module is not to mess with the current Islandora's implementation. If you don't code anything extra, then you won't even notice this module.  

* Q. My XML Form used in an Ingest Step does not transform to DC.
  * A. XML forms used in Ingest Steps are not considered for XLST transformations. The main idea is to complement the Main Metadata Form. This one is responsible for transforming to DC and also gives the Object a Title. Further XML metadaforms only fill their correspondent Datastreams

* Q. When editing an Datastream the Object title is not updated(and my DC also!), even if i defined a title field when associating my form.
  * A. Thats the idea. This Module intercepts every datastream edit call and checks if the particular datastream was created by the primary metadatastep (the one that sets title and makes XSLT transforms) or not. It would be very bad if every Form would overwrite by its own the current Object's title!

* Q. I get errors!(lots, many, only some notices).
  * A. 
    1. First step is to check if the 'args' array passed to islandora_more_metadata_xml_forms_additional_ingest_form is wellformed and the form referenced by this array really exist. I suggest debuging or using the recomended hook to get this array right. 
    2. Second Step is to make sure you have different DSID for every Ingest Step. It's obvious but does not hurt to check. If you define the same DSID for two consecutive steps, one will overwrite the next.

* Q. It seems a little slower when hitting back and next.
  * A. True. Current implementation of Objective Forms has space for only one preprocessed XML form. So every time you hit back and forth, the XML form must be rebuild. Still thinking on how make my own caching there.


## Troubleshooting/Issues

Still having problems or solved a problem? Write me (Diego Pino Navarro), or Check out the Islandora google groups for a solution (i'm most of the time around).

* [Islandora Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora)
* [Islandora Dev Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora-dev)
* Note if you get error "XML form definition is not valid." during ingest you need to update your libxml2 version to 2.7+
* Additional documentation on using XML Forms can be found [here](https://github.com/Islandora/islandora/wiki/Working-Programmatically-With-XML-Forms).



## Maintainer

Current maintainer:

* [Diego Pino](https://github.com/diegopino)

## Development

If you would like to contribute to this module, just ask. Need help using. Ask too!

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)

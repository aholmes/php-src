#include "php_soap.h"

typedef struct sdlCtx {
	sdlPtr root;
	HashTable messages;
	HashTable bindings;
	HashTable portTypes;
	HashTable services;
} sdlCtx;

static void delete_binding(void *binding);
static void delete_function(void *function);
static void delete_paramater(void *paramater);
static void delete_document(void *doc_ptr);

encodePtr get_encoder_from_prefix(sdlPtr sdl, xmlNodePtr data, const char *type)
{
	encodePtr enc = NULL;
	TSRMLS_FETCH();

	enc = get_conversion_from_type(data, type);
	if (enc == NULL && sdl) {
		enc = get_conversion_from_type_ex(sdl->encoders, data, type);
	}
	if (enc == NULL) {
		enc = get_conversion(UNKNOWN_TYPE);
	}
	return enc;
}

encodePtr get_encoder(sdlPtr sdl, const char *ns, const char *type)
{
	encodePtr enc = NULL;
	char *nscat;
	TSRMLS_FETCH();

	nscat = emalloc(strlen(ns) + strlen(type) + 2);
	sprintf(nscat, "%s:%s", ns, type);

	enc = get_encoder_ex(sdl, nscat);

	efree(nscat);
	return enc;
}

encodePtr get_encoder_ex(sdlPtr sdl, const char *nscat)
{
	encodePtr enc = NULL;
	TSRMLS_FETCH();

	enc = get_conversion_from_href_type(nscat);
	if (enc == NULL && sdl) {
		enc = get_conversion_from_href_type_ex(sdl->encoders, nscat, strlen(nscat));
	}
	if (enc == NULL) {
		enc = get_conversion(UNKNOWN_TYPE);
	}
	return enc;
}

encodePtr get_create_encoder(sdlPtr sdl, sdlTypePtr cur_type, const char *ns, const char *type)
{
	encodePtr enc = NULL;
	smart_str nscat = {0};
	TSRMLS_FETCH();

	smart_str_appends(&nscat, ns);
	smart_str_appendc(&nscat, ':');
	smart_str_appends(&nscat, type);
	smart_str_0(&nscat);

	enc = get_conversion_from_href_type(nscat.c);
	if (enc == NULL) {
		enc = get_conversion_from_href_type_ex(sdl->encoders, nscat.c, nscat.len);
	}
	if (enc == NULL) {
		enc = create_encoder(sdl, cur_type, ns, type);
	}

	smart_str_free(&nscat);
	return enc;
}

encodePtr create_encoder(sdlPtr sdl, sdlTypePtr cur_type, const char *ns, const char *type)
{
	smart_str nscat = {0};
	encodePtr enc, *enc_ptr;

	if (sdl->encoders == NULL) {
		sdl->encoders = malloc(sizeof(HashTable));
		zend_hash_init(sdl->encoders, 0, NULL, delete_encoder, 1);
	}
	smart_str_appends(&nscat, ns);
	smart_str_appendc(&nscat, ':');
	smart_str_appends(&nscat, type);
	smart_str_0(&nscat);
	if (zend_hash_find(sdl->encoders, nscat.c, nscat.len + 1, (void**)&enc_ptr) == SUCCESS) {
		enc = *enc_ptr;
	} else {
		enc_ptr = NULL;
		enc = malloc(sizeof(encode));
	}
	memset(enc, 0, sizeof(encode));

	enc->details.ns = strdup(ns);
	enc->details.type_str = strdup(type);
	enc->details.sdl_type = cur_type;
	enc->to_xml = sdl_guess_convert_xml;
	enc->to_zval = sdl_guess_convert_zval;
	
	if (enc_ptr == NULL) {
		zend_hash_update(sdl->encoders, nscat.c, nscat.len + 1, &enc, sizeof(encodePtr), NULL);
	}
	smart_str_free(&nscat);
	return enc;
}

static xmlNodePtr sdl_to_xml_object(encodeType enc_type, zval *data, int style)
{
	sdlTypePtr type = enc_type.sdl_type;
	xmlNodePtr ret;
	sdlTypePtr *t, tmp;
	TSRMLS_FETCH();

	ret = xmlNewNode(NULL, "BOGUS");
	FIND_ZVAL_NULL(data, ret, style);

	if (Z_TYPE_P(data) != IS_OBJECT) {
		return ret;
	}

	zend_hash_internal_pointer_reset(type->elements);
	while (zend_hash_get_current_data(type->elements, (void **)&t) != FAILURE) {
		zval **prop;
 		tmp = *t;
		if (zend_hash_find(Z_OBJPROP_P(data), tmp->name, strlen(tmp->name) + 1, (void **)&prop) == FAILURE) {
			if (tmp->nillable == FALSE)
				php_error(E_ERROR, "Error encoding object to xml missing property \"%s\"", tmp->name);
		} else {
			xmlNodePtr newNode;
			encodePtr enc;

			if (tmp->encode) {
				enc = tmp->encode;
			} else {
				enc = get_conversion((*prop)->type);
			}
			newNode = master_to_xml(enc, (*prop), style);
			xmlNodeSetName(newNode, tmp->name);
			xmlAddChild(ret, newNode);
		}
		zend_hash_move_forward(type->elements);
	}
	if (style == SOAP_ENCODED) {
		set_ns_and_type(ret, enc_type);
	}

	return ret;
}

static void add_xml_array_elements(xmlNodePtr xmlParam,
                                   sdlTypePtr type,
                                   encodePtr enc,
                                   int dimension ,
                                   int* dims,
                                   zval* data,
                                   int style)
{
	int j;

	if (Z_TYPE_P(data) == IS_ARRAY) {
	 	zend_hash_internal_pointer_reset(data->value.ht);
		for (j=0; j<dims[0]; j++) {
 			zval **zdata;
 			zend_hash_get_current_data(data->value.ht, (void **)&zdata);
 			if (dimension == 1) {
	 			xmlNodePtr xparam;
 				if (enc == NULL) {
					TSRMLS_FETCH();
 					xparam = master_to_xml(get_conversion((*zdata)->type), (*zdata), style);
 				} else {
 					xparam = master_to_xml(enc, (*zdata), style);
	 			}

 				xmlNodeSetName(xparam, type->name);
 				xmlAddChild(xmlParam, xparam);
 			} else {
 			  add_xml_array_elements(xmlParam, type, enc, dimension-1, dims+1, *zdata, style);
 			}
 			zend_hash_move_forward(data->value.ht);
 		}
 	}
}

static xmlNodePtr sdl_to_xml_array(encodeType enc_type, zval *data, int style)
{
	sdlTypePtr type = enc_type.sdl_type;
	smart_str array_type_and_size = {0}, array_type = {0};
	int i;
	int dimension = 1;
	int* dims;
	xmlNodePtr xmlParam;
	sdlTypePtr elementType;
	encodePtr enc = NULL;
	TSRMLS_FETCH();

	xmlParam = xmlNewNode(NULL,"BOGUS");

	FIND_ZVAL_NULL(data, xmlParam, style);

	if (Z_TYPE_P(data) == IS_ARRAY) {
		sdlAttributePtr *arrayType;
		i = zend_hash_num_elements(Z_ARRVAL_P(data));

		if (style == SOAP_ENCODED) {
			xmlAttrPtr *wsdl;
			if (type->attributes &&
				zend_hash_find(type->attributes, SOAP_ENC_NAMESPACE":arrayType",
					sizeof(SOAP_ENC_NAMESPACE":arrayType"),
					(void **)&arrayType) == SUCCESS &&
				zend_hash_find((*arrayType)->extraAttributes, WSDL_NAMESPACE":arrayType", sizeof(WSDL_NAMESPACE":arrayType"), (void **)&wsdl) == SUCCESS) {

				char *ns = NULL, *value, *end;
				xmlNsPtr myNs;
				zval** el;

				parse_namespace((*wsdl)->children->content, &value, &ns);
				myNs = xmlSearchNs((*wsdl)->doc, (*wsdl)->parent, ns);

				end = strrchr(value,'[');
				if (end) {
					*end = '\0';
					end++;
					while (*end != ']' && *end != '\0') {
						if (*end == ',') {
							dimension++;
						}
						end++;
					}
				}
				if (myNs != NULL) {
					enc = get_encoder(SOAP_GLOBAL(sdl), myNs->href, value);

					if (strcmp(myNs->href,XSD_NAMESPACE) == 0) {
						smart_str_appendl(&array_type_and_size, XSD_NS_PREFIX, sizeof(XSD_NS_PREFIX) - 1);
						smart_str_appendc(&array_type_and_size, ':');
					} else {
						smart_str *prefix = encode_new_ns();
						smart_str smart_ns = {0};

						smart_str_appendl(&smart_ns, "xmlns:", sizeof("xmlns:") - 1);
						smart_str_appendl(&smart_ns, prefix->c, prefix->len);
						smart_str_0(&smart_ns);
						xmlSetProp(xmlParam, smart_ns.c, myNs->href);
						smart_str_free(&smart_ns);

						smart_str_appends(&array_type_and_size, prefix->c);
						smart_str_appendc(&array_type_and_size, ':');
						smart_str_free(prefix);
						efree(prefix);
					}
				}

				dims = emalloc(sizeof(int)*dimension);
				dims[0] = i;
				el = &data;
				for (i = 1; i < dimension; i++) {
					if (el != NULL && Z_TYPE_PP(el) == IS_ARRAY && Z_ARRVAL_PP(el)->pListHead) {
						el = (zval**)Z_ARRVAL_PP(el)->pListHead->pData;
						if (Z_TYPE_PP(el) == IS_ARRAY) {
							dims[i] = zend_hash_num_elements(Z_ARRVAL_PP(el));
						} else {
						  dims[i] = 0;
						}
					}
				}

				smart_str_appends(&array_type_and_size, value);
				smart_str_appendc(&array_type_and_size, '[');
				smart_str_append_long(&array_type_and_size, dims[0]);
				for (i=1; i<dimension; i++) {
					smart_str_appendc(&array_type_and_size, ',');
					smart_str_append_long(&array_type_and_size, dims[i]);
				}
				smart_str_appendc(&array_type_and_size, ']');
				smart_str_0(&array_type_and_size);

				efree(value);
				if (ns) efree(ns);
			} else if (type->elements &&
			           zend_hash_num_elements(type->elements) == 1 &&
			           (elementType = *(sdlTypePtr*)type->elements->pListHead->pData) != NULL &&
			           elementType->encode && elementType->encode->details.type_str) {
				char* ns = elementType->encode->details.ns;

				if (ns) {
					if (strcmp(ns,XSD_NAMESPACE) == 0) {
						smart_str_appendl(&array_type_and_size, XSD_NS_PREFIX, sizeof(XSD_NS_PREFIX) - 1);
						smart_str_appendc(&array_type_and_size, ':');
					} else {
						smart_str *prefix = encode_new_ns();
						smart_str smart_ns = {0};

						smart_str_appendl(&smart_ns, "xmlns:", sizeof("xmlns:") - 1);
						smart_str_appendl(&smart_ns, prefix->c, prefix->len);
						smart_str_0(&smart_ns);
						xmlSetProp(xmlParam, smart_ns.c, ns);
						smart_str_free(&smart_ns);

						smart_str_appends(&array_type_and_size, prefix->c);
						smart_str_appendc(&array_type_and_size, ':');
						smart_str_free(prefix);
						efree(prefix);
					}
				}
				enc = elementType->encode;
				smart_str_appends(&array_type_and_size, elementType->encode->details.type_str);
				smart_str_free(&array_type);
				smart_str_appendc(&array_type_and_size, '[');
				smart_str_append_long(&array_type_and_size, i);
				smart_str_appendc(&array_type_and_size, ']');
				smart_str_0(&array_type_and_size);

				dims = emalloc(sizeof(int)*dimension);
				dims[0] = i;
			} else {
				smart_str array_type = {0};
				get_array_type(data, &array_type TSRMLS_CC);
				enc = get_encoder_ex(SOAP_GLOBAL(sdl), array_type.c);
				smart_str_append(&array_type_and_size, &array_type);
				smart_str_free(&array_type);
				smart_str_appendc(&array_type_and_size, '[');
				smart_str_append_long(&array_type_and_size, i);
				smart_str_appendc(&array_type_and_size, ']');
				smart_str_0(&array_type_and_size);
				dims = emalloc(sizeof(int)*dimension);
				dims[0] = i;
			}
			xmlSetProp(xmlParam, SOAP_ENC_NS_PREFIX":arrayType", array_type_and_size.c);

			smart_str_free(&array_type_and_size);
			smart_str_free(&array_type);
		} else {
			dims = emalloc(sizeof(int)*dimension);
			dims[0] = i;
		}

		add_xml_array_elements(xmlParam, type, enc, dimension, dims, data, style);
		efree(dims);
		if (style == SOAP_ENCODED) {
			set_ns_and_type(xmlParam, enc_type);
		}
	}

	return xmlParam;
}

zval *sdl_guess_convert_zval(encodeType enc, xmlNodePtr data)
{
	sdlTypePtr type;

	type = enc.sdl_type;
	if (type->encode) {
		if (type->encode->details.type == IS_ARRAY ||
			type->encode->details.type == SOAP_ENC_ARRAY) {
			return to_zval_array(enc, data);
		} else if (type->encode->details.type == IS_OBJECT ||
			type->encode->details.type == SOAP_ENC_OBJECT) {
			return to_zval_object(enc, data);
		} else {
			if (memcmp(&type->encode->details,&enc,sizeof(enc))!=0) {
				return master_to_zval(type->encode, data);
			} else {
				TSRMLS_FETCH();
				return master_to_zval(get_conversion(UNKNOWN_TYPE), data);
			}
		}
	} else if (type->elements) {
		return to_zval_object(enc, data);
	}	else {
		return guess_zval_convert(enc, data);
	}
}

xmlNodePtr sdl_guess_convert_xml(encodeType enc, zval *data, int style)
{
	sdlTypePtr type;
	xmlNodePtr ret = NULL;

	type = enc.sdl_type;

	if (type->restrictions) {
		if (type->restrictions->enumeration) {
			if (Z_TYPE_P(data) == IS_STRING) {
				if (!zend_hash_exists(type->restrictions->enumeration,Z_STRVAL_P(data),Z_STRLEN_P(data)+1)) {
					php_error(E_WARNING,"Restriction: invalid enumeration value \"%s\".",Z_STRVAL_P(data));
				}
			}
		}
	}

	if (type->encode) {
		if (type->encode->details.type == IS_ARRAY ||
			type->encode->details.type == SOAP_ENC_ARRAY) {
			ret = sdl_to_xml_array(enc, data, style);
		} else if (type->encode->details.type == IS_OBJECT ||
			type->encode->details.type == SOAP_ENC_OBJECT) {
			ret = sdl_to_xml_object(enc, data, style);
		} else {
			if (memcmp(&type->encode->details,&enc,sizeof(enc))!=0) {
				ret = master_to_xml(type->encode, data, style);
			} else {
				TSRMLS_FETCH();
				ret = master_to_xml(get_conversion(UNKNOWN_TYPE), data, style);
			}
		}
	}
	else if (type->elements) {
		ret = sdl_to_xml_object(enc, data, style);
	}	else {
		ret = guess_xml_convert(enc, data, style);
	}
	return ret;
}

sdlBindingPtr get_binding_from_type(sdlPtr sdl, int type)
{
	sdlBindingPtr *binding;

	if (sdl == NULL) {
		return NULL;
	}

	for (zend_hash_internal_pointer_reset(sdl->bindings);
		zend_hash_get_current_data(sdl->bindings, (void **) &binding) == SUCCESS;
		zend_hash_move_forward(sdl->bindings)) {
		if ((*binding)->bindingType == type) {
			return *binding;
		}
	}
	return NULL;
}

sdlBindingPtr get_binding_from_name(sdlPtr sdl, char *name, char *ns)
{
	sdlBindingPtr binding = NULL;
	smart_str key = {0};

	smart_str_appends(&key, ns);
	smart_str_appendc(&key, ':');
	smart_str_appends(&key, name);
	smart_str_0(&key);

	zend_hash_find(sdl->bindings, key.c, key.len, (void **)&binding);

	smart_str_free(&key);
	return binding;
}

static void load_wsdl_ex(char *struri, sdlCtx *ctx, int include)
{
	sdlPtr tmpsdl = ctx->root;
	xmlDocPtr wsdl;
	xmlNodePtr root, definitions, trav;
	xmlAttrPtr targetNamespace;
	int old_error_reporting;

	/* TODO: WSDL Caching */

	old_error_reporting = EG(error_reporting);
	EG(error_reporting) &= ~(E_WARNING|E_NOTICE|E_USER_WARNING|E_USER_NOTICE);

	wsdl = xmlParseFile(struri);
	xmlCleanupParser();

	EG(error_reporting) = old_error_reporting;


	if (!wsdl) {
		php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Couldn't load from %s", struri);
	}

	zend_hash_next_index_insert(&tmpsdl->docs, (void**)&wsdl, sizeof(xmlDocPtr), NULL);

	root = wsdl->children;
	definitions = get_node(root, "definitions");
	if (!definitions) {
		if (include) {
			xmlNodePtr schema = get_node(root, "schema");
			if (schema) {
				load_schema(&tmpsdl, schema);
				return;
			}
		}
		php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: Couldn't find \"definitions\" in %s", struri);
	}

	if (!include) {
		targetNamespace = get_attribute(definitions->properties, "targetNamespace");
		if (targetNamespace) {
			tmpsdl->target_ns = strdup(targetNamespace->children->content);
		}
	}

	trav = definitions->children;
	while (trav != NULL) {
		if (trav->type == XML_ELEMENT_NODE) {
			if (strcmp(trav->name,"types") == 0) {
				/* TODO: Only one "types" is allowed */
				xmlNodePtr trav2 = trav->children;
				xmlNodePtr schema;
				FOREACHNODE(trav2, "schema", schema) {
					load_schema(&tmpsdl, schema);
				}
				ENDFOREACH(trav2);

			} else if (strcmp(trav->name,"import") == 0) {
				/* TODO: namespace ??? */
				xmlAttrPtr tmp = get_attribute(trav->properties, "location");
				if (tmp) {
					load_wsdl_ex(tmp->children->content, ctx, 1);
				}

			} else if (strcmp(trav->name,"message") == 0) {
				xmlAttrPtr name = get_attribute(trav->properties, "name");
				if (name && name->children && name->children->content) {
					zend_hash_add(&ctx->messages, name->children->content, strlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL);
					/* TODO: redeclaration handling */
				} else {
					php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <message> hasn't name attribute");
				}

			} else if (strcmp(trav->name,"portType") == 0) {
				xmlAttrPtr name = get_attribute(trav->properties, "name");
				if (name && name->children && name->children->content) {
					zend_hash_add(&ctx->portTypes, name->children->content, strlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL);
					/* TODO: redeclaration handling */
				} else {
					php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <portType> hasn't name attribute");
				}

			} else if (strcmp(trav->name,"binding") == 0) {
				xmlAttrPtr name = get_attribute(trav->properties, "name");
				if (name && name->children && name->children->content) {
					zend_hash_add(&ctx->bindings, name->children->content, strlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL);
					/* TODO: redeclaration handling */
				} else {
					php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <binding> hasn't name attribute");
				}

			} else if (strcmp(trav->name,"service") == 0) {
				xmlAttrPtr name = get_attribute(trav->properties, "name");
				if (name && name->children && name->children->content) {
					zend_hash_add(&ctx->services, name->children->content, strlen(name->children->content)+1,&trav, sizeof(xmlNodePtr), NULL);
					/* TODO: redeclaration handling */
				} else {
					php_error(E_ERROR,"SOAP-ERROR: Parsing WSDL: <service> hasn't name attribute");
				}

			} else {
			  /* TODO: extensibility elements */
			}
		}
		trav = trav->next;
	}
}

static sdlPtr load_wsdl(char *struri)
{
	sdlCtx ctx;
	int i,n;

	ctx.root = malloc(sizeof(sdl));
	memset(ctx.root, 0, sizeof(sdl));
	ctx.root->source = strdup(struri);
	zend_hash_init(&ctx.root->docs, 0, NULL, delete_document, 1);
	zend_hash_init(&ctx.root->functions, 0, NULL, delete_function, 1);

	zend_hash_init(&ctx.messages, 0, NULL, NULL, 0);
	zend_hash_init(&ctx.bindings, 0, NULL, NULL, 0);
	zend_hash_init(&ctx.portTypes, 0, NULL, NULL, 0);
	zend_hash_init(&ctx.services,  0, NULL, NULL, 0);

	load_wsdl_ex(struri,&ctx, 0);

	n = zend_hash_num_elements(&ctx.services);
	if (n > 0) {
		zend_hash_internal_pointer_reset(&ctx.services);
		for (i = 0; i < n; i++) {
			xmlNodePtr *tmp, service;
			xmlAttrPtr name;
			xmlNodePtr trav, port;

			zend_hash_get_current_data(&ctx.services, (void **)&tmp);
			service = *tmp;

			name = get_attribute(service->properties, "name");
			if (!name) {
				php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No name associated with service");
			}

			trav = service->children;
			FOREACHNODE(trav, "port", port) {
				xmlAttrPtr type, name, bindingAttr, location;
				xmlNodePtr portType, operation;
				xmlNodePtr address, binding, trav2;
				char *ns, *ctype;
				sdlBindingPtr tmpbinding;

				tmpbinding = malloc(sizeof(sdlBinding));
				memset(tmpbinding, 0, sizeof(sdlBinding));

				name = get_attribute(port->properties, "name");
				if (!name) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No name associated with port");
				}

				bindingAttr = get_attribute(port->properties, "binding");
				if (bindingAttr == NULL) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No binding associated with port");
				}

				/* find address and figure out binding type */
				address = get_node(port->children, "address");
				if (!address) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No address associated with port");
				}

				location = get_attribute(address->properties, "location");
				if (!location) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No location associated with port");
				}

				tmpbinding->location = strdup(location->children->content);

				if (address->ns && !strcmp(address->ns->href, WSDL_SOAP_NAMESPACE)) {
					tmpbinding->bindingType = BINDING_SOAP;
				} else if (address->ns && !strcmp(address->ns->href, WSDL_HTTP_NAMESPACE)) {
					tmpbinding->bindingType = BINDING_HTTP;
				}

				parse_namespace(bindingAttr->children->content, &ctype, &ns);
				if (zend_hash_find(&ctx.bindings, ctype, strlen(ctype)+1, (void*)&tmp) != SUCCESS) {
					php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: No binding element with name \"%s\"", ctype);
				}
				binding = *tmp;

				if (ns) {efree(ns);}
				if (ctype) {efree(ctype);}

				if (tmpbinding->bindingType == BINDING_SOAP) {
					sdlSoapBindingPtr soapBinding;
					xmlNodePtr soapBindingNode;
					xmlAttrPtr tmp;

					soapBinding = malloc(sizeof(sdlSoapBinding));
					memset(soapBinding, 0, sizeof(sdlSoapBinding));

					soapBindingNode = get_node_ex(binding->children, "binding", WSDL_SOAP_NAMESPACE);
					if (soapBindingNode) {
						tmp = get_attribute(soapBindingNode->properties, "style");
						if (tmp && !strcmp(tmp->children->content, "document")) {
							soapBinding->style = SOAP_DOCUMENT;
						} else {
							soapBinding->style = SOAP_RPC;
						}

						tmp = get_attribute(soapBindingNode->properties, "transport");
						if (tmp) {
							if (strcmp(tmp->children->content, WSDL_HTTP_TRANSPORT)) {
								php_error(E_ERROR, "SOAP-ERROR: Parsing WSDL: PHP-SOAP doesn't support transport '%s'", tmp->children->content);
							}
							soapBinding->transport = strdup(tmp->children->content);
						}
						tmpbinding->bindingAttributes = (void *)soapBinding;
					}
				}

				name = get_attribute(binding->properties, "name");
				if (name == NULL) {
					php_error(E_ERROR, "Error parsing wsdl (Missing \"name\" attribute for \"binding\")");
				}

				tmpbinding->name = strdup(name->children->content);

				type = get_attribute(binding->properties, "type");
				if (type == NULL) {
					php_error(E_ERROR, "Error parsing wsdl (Missing \"type\" attribute for \"binding\")");
				}
				parse_namespace(type->children->content, &ctype, &ns);

				if (zend_hash_find(&ctx.portTypes, ctype, strlen(ctype)+1, (void**)&tmp) != SUCCESS) {
					php_error(E_ERROR, "Error parsing wsdl (Missing \"portType\" with name \"%s\")", name->children->content);
				}
				portType = *tmp;

				if (ctype) {efree(ctype);}
				if (ns) {efree(ns);}

				trav2 = binding->children;
				FOREACHNODE(trav2, "operation", operation) {
					sdlFunctionPtr function;
					xmlNodePtr input, output, fault, portTypeOperation, portTypeInput, msgInput, msgOutput;
					xmlAttrPtr op_name, paramOrder;

					op_name = get_attribute(operation->properties, "name");
					if (op_name == NULL) {
						php_error(E_ERROR, "Error parsing wsdl (Missing \"name\" attribute for \"operation\")");
					}

					portTypeOperation = get_node_with_attribute(portType->children, "operation", "name", op_name->children->content);
					if (portTypeOperation == NULL) {
						php_error(E_ERROR, "Error parsing wsdl (Missing \"portType/operation\" with name \"%s\")", op_name->children->content);
					}

					function = malloc(sizeof(sdlFunction));
					function->functionName = strdup(op_name->children->content);
					function->requestParameters = NULL;
					function->responseParameters = NULL;
					function->responseName = NULL;
					function->requestName = NULL;
					function->bindingAttributes = NULL;

					if (tmpbinding->bindingType == BINDING_SOAP) {
						sdlSoapBindingFunctionPtr soapFunctionBinding;
						sdlSoapBindingPtr soapBinding;
						xmlNodePtr soapOperation;
						xmlAttrPtr tmp;

						soapFunctionBinding = malloc(sizeof(sdlSoapBindingFunction));
						memset(soapFunctionBinding, 0, sizeof(sdlSoapBindingFunction));
						soapBinding = (sdlSoapBindingPtr)tmpbinding->bindingAttributes;
						soapFunctionBinding->style = soapBinding->style;

						soapOperation = get_node_ex(operation->children, "operation", WSDL_SOAP_NAMESPACE);
						if (soapOperation) {
							tmp = get_attribute(soapOperation->properties, "soapAction");
							if (tmp) {
								soapFunctionBinding->soapAction = strdup(tmp->children->content);
							}

							tmp = get_attribute(soapOperation->properties, "style");
							if (tmp && !strcmp(tmp->children->content, "rpc")) {
								soapFunctionBinding->style = SOAP_RPC;
							} else if (!soapBinding->style) {
								soapFunctionBinding->style = SOAP_DOCUMENT;
							}
						}

						function->bindingAttributes = (void *)soapFunctionBinding;
					}

					input = get_node(operation->children, "input");
					if (input != NULL) {
						xmlAttrPtr message;
						xmlNodePtr part, trav3;
						char *ns, *ctype;

						portTypeInput = get_node(portTypeOperation->children, "input");
						if (portTypeInput) {
							message = get_attribute(portTypeInput->properties, "message");
							if (message == NULL) {
								php_error(E_ERROR, "Error parsing wsdl (Missing name for \"input\" of \"%s\")", op_name->children->content);
							}

							function->requestName = strdup(function->functionName);
							function->requestParameters = malloc(sizeof(HashTable));
							zend_hash_init(function->requestParameters, 0, NULL, delete_paramater, 1);
		
							parse_namespace(message->children->content, &ctype, &ns);

							if (zend_hash_find(&ctx.messages, ctype, strlen(ctype)+1, (void**)&tmp) != SUCCESS) {
								php_error(E_ERROR, "Error parsing wsdl (Missing \"message\" with name \"%s\")", message->children->content);
							}
							msgInput = *tmp;

							if (ctype) {efree(ctype);}
							if (ns) {efree(ns);}

							if (tmpbinding->bindingType == BINDING_SOAP) {
								sdlSoapBindingFunctionPtr soapFunctionBinding = function->bindingAttributes;
								xmlNodePtr body;
								xmlAttrPtr tmp;
				
								body = get_node_ex(input->children, "body", WSDL_SOAP_NAMESPACE);
								if (body) {
									tmp = get_attribute(body->properties, "use");
									if (tmp && !strcmp(tmp->children->content, "literal")) {
										soapFunctionBinding->input.use = SOAP_LITERAL;
									} else {
										soapFunctionBinding->input.use = SOAP_ENCODED;
									}
						
									tmp = get_attribute(body->properties, "namespace");
									if (tmp) {
										soapFunctionBinding->input.ns = strdup(tmp->children->content);
									}

									tmp = get_attribute(body->properties, "parts");
									if (tmp) {
										soapFunctionBinding->input.parts = strdup(tmp->children->content);
									}

									tmp = get_attribute(body->properties, "encodingStyle");
									if (tmp) {
										soapFunctionBinding->input.encodingStyle = strdup(tmp->children->content);
									}
								}
							}

							trav3 = msgInput->children;
							FOREACHNODE(trav3, "part", part) {
								xmlAttrPtr element, type, name;
								sdlParamPtr param;

								param = malloc(sizeof(sdlParam));
								param->order = 0;

								name = get_attribute(part->properties, "name");
								if (name == NULL) {
									php_error(E_ERROR, "Error parsing wsdl (No name associated with part \"%s\")", msgInput->name);
								}

								param->paramName = strdup(name->children->content);

								element = get_attribute(part->properties, "element");
								if (element != NULL) {
									param->encode = get_encoder_from_prefix(ctx.root, part, element->children->content);
								}

								type = get_attribute(part->properties, "type");
								if (type != NULL) {
									param->encode = get_encoder_from_prefix(ctx.root, part, type->children->content);
								}

								zend_hash_next_index_insert(function->requestParameters, &param, sizeof(sdlParamPtr), NULL);
							}
							ENDFOREACH(trav3);
						}

						paramOrder = get_attribute(portTypeOperation->properties, "parameterOrder");
						if (paramOrder) {

						}
					}

					output = get_node(portTypeOperation->children, "output");
					if (output != NULL) {
						xmlAttrPtr message;
						xmlNodePtr part, trav3;
						char *ns, *ctype;

						function->responseName = malloc(strlen(function->functionName) + strlen("Response") + 1);
						sprintf(function->responseName, "%sResponse", function->functionName);
						function->responseParameters = malloc(sizeof(HashTable));
						zend_hash_init(function->responseParameters, 0, NULL, delete_paramater, 1);

						message = get_attribute(output->properties, "message");
						if (message == NULL) {
							php_error(E_ERROR, "Error parsing wsdl (Missing name for \"output\" of \"%s\")", op_name->children->content);
						}

						parse_namespace(message->children->content, &ctype, &ns);
						if (zend_hash_find(&ctx.messages, ctype, strlen(ctype)+1, (void**)&tmp) != SUCCESS) {
							php_error(E_ERROR, "Error parsing wsdl (Missing \"message\" with name \"%s\")", message->children->content);
						}
						msgOutput = *tmp;

						if (ctype) {efree(ctype);}
						if (ns) {efree(ns);}

						if (tmpbinding->bindingType == BINDING_SOAP) {
							sdlSoapBindingFunctionPtr soapFunctionBinding = function->bindingAttributes;
							xmlNodePtr body;
							xmlAttrPtr tmp;

							body = get_node_ex(output->children, "body", WSDL_SOAP_NAMESPACE);
							if (body) {
								tmp = get_attribute(body->properties, "use");
								if (tmp && !strcmp(tmp->children->content, "literal")) {
									soapFunctionBinding->output.use = SOAP_LITERAL;
								} else {
									soapFunctionBinding->output.use = SOAP_ENCODED;
								}

								tmp = get_attribute(body->properties, "namespace");
								if (tmp) {
									soapFunctionBinding->output.ns = strdup(tmp->children->content);
								}

								tmp = get_attribute(body->properties, "parts");
								if (tmp) {
									soapFunctionBinding->output.parts = strdup(tmp->children->content);
								}

								tmp = get_attribute(body->properties, "encodingStyle");
								if (tmp) {
									soapFunctionBinding->output.encodingStyle = strdup(tmp->children->content);
								}
							}
						}

						trav3 = msgOutput->children;
						FOREACHNODE(trav3, "part", part) {
							sdlParamPtr param;
							xmlAttrPtr element, type, name;

							param = malloc(sizeof(sdlParam));
							param->order = 0;

							name = get_attribute(part->properties, "name");
							if (name == NULL) {
								php_error(E_ERROR, "Error parsing wsdl (No name associated with part \"%s\")", msgOutput->name);
							}

							param->paramName = strdup(name->children->content);

							element = get_attribute(part->properties, "element");
							if (element) {
								param->encode = get_encoder_from_prefix(ctx.root, part, element->children->content);
							}

							type = get_attribute(part->properties, "type");
							if (type) {
								param->encode = get_encoder_from_prefix(ctx.root, part, type->children->content);
							}

							zend_hash_next_index_insert(function->responseParameters, &param, sizeof(sdlParamPtr), NULL);
						}
						ENDFOREACH(trav3);
					}

					fault = get_node(operation->children, "fault");
					if (!fault) {
					}

					function->binding = tmpbinding;
					zend_hash_add(&ctx.root->functions, php_strtolower(function->functionName, strlen(function->functionName)), strlen(function->functionName), &function, sizeof(sdlFunctionPtr), NULL);
				}
				ENDFOREACH(trav2);

				if (!ctx.root->bindings) {
					ctx.root->bindings = malloc(sizeof(HashTable));
					zend_hash_init(ctx.root->bindings, 0, NULL, delete_binding, 1);
				}

				zend_hash_add(ctx.root->bindings, tmpbinding->name, strlen(tmpbinding->name), &tmpbinding, sizeof(sdlBindingPtr), NULL);
			}
			ENDFOREACH(trav);

			zend_hash_move_forward(&ctx.services);
		}
	} else {
		php_error(E_ERROR, "Error parsing wsdl (\"Couldn't bind to service\")");
	}

	zend_hash_destroy(&ctx.messages);
	zend_hash_destroy(&ctx.bindings);
	zend_hash_destroy(&ctx.portTypes);
	zend_hash_destroy(&ctx.services);

	return ctx.root;
}

sdlPtr get_sdl(char *uri)
{
	sdlPtr tmp, *hndl;
	TSRMLS_FETCH();

	tmp = NULL;
	hndl = NULL;
	if (zend_hash_find(SOAP_GLOBAL(sdls), uri, strlen(uri), (void **)&hndl) == FAILURE) {
		tmp = load_wsdl(uri);
		zend_hash_add(SOAP_GLOBAL(sdls), uri, strlen(uri), &tmp, sizeof(sdlPtr), NULL);
	} else {
		tmp = *hndl;
	}

	return tmp;
}

/* Deletes */
void delete_sdl(void *handle)
{
	sdlPtr tmp = *((sdlPtr*)handle);

	zend_hash_destroy(&tmp->docs);
	zend_hash_destroy(&tmp->functions);
	if (tmp->source) {
		free(tmp->source);
	}
	if (tmp->target_ns) {
		free(tmp->target_ns);
	}
	if (tmp->encoders) {
		zend_hash_destroy(tmp->encoders);
		free(tmp->encoders);
	}
	if (tmp->types) {
		zend_hash_destroy(tmp->types);
		free(tmp->types);
	}
	if (tmp->bindings) {
		zend_hash_destroy(tmp->bindings);
		free(tmp->bindings);
	}
	free(tmp);
}

static void delete_binding(void *data)
{
	sdlBindingPtr binding = *((sdlBindingPtr*)data);

	if (binding->location) {
		free(binding->location);
	}
	if (binding->name) {
		free(binding->name);
	}

	if (binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingPtr soapBind = binding->bindingAttributes;
		free(soapBind->transport);
	}
}

static void delete_sdl_soap_binding_function_body(sdlSoapBindingFunctionBody body)
{
	if (body.ns) {
		free(body.ns);
	}
	if (body.parts) {
		free(body.parts);
	}
	if (body.encodingStyle) {
		free(body.encodingStyle);
	}
}

static void delete_function(void *data)
{
	sdlFunctionPtr function = *((sdlFunctionPtr*)data);

	if (function->functionName) {
		free(function->functionName);
	}
	if (function->requestName) {
		free(function->requestName);
	}
	if (function->responseName) {
		free(function->responseName);
	}
	if (function->requestParameters) {
		zend_hash_destroy(function->requestParameters);
		free(function->requestParameters);
	}
	if (function->responseParameters) {
		zend_hash_destroy(function->responseParameters);
		free(function->responseParameters);
	}

	if (function->bindingAttributes &&
	    function->binding && function->binding->bindingType == BINDING_SOAP) {
		sdlSoapBindingFunctionPtr soapFunction = function->bindingAttributes;
		if (soapFunction->soapAction) {
			free(soapFunction->soapAction);
		}
		delete_sdl_soap_binding_function_body(soapFunction->input);
		delete_sdl_soap_binding_function_body(soapFunction->output);
		delete_sdl_soap_binding_function_body(soapFunction->falut);
	}
}

static void delete_paramater(void *data)
{
	sdlParamPtr param = *((sdlParamPtr*)data);
	if (param->paramName) {
		free(param->paramName);
	}
	free(param);
}

void delete_mapping(void *data)
{
	soapMappingPtr map = (soapMappingPtr)data;

	if (map->ns) {
		efree(map->ns);
	}
	if (map->ctype) {
		efree(map->ctype);
	}

	if (map->type == SOAP_MAP_FUNCTION) {
		if (map->map_functions.to_xml_before) {
			zval_ptr_dtor(&map->map_functions.to_xml_before);
		}
		if (map->map_functions.to_xml) {
			zval_ptr_dtor(&map->map_functions.to_xml);
		}
		if (map->map_functions.to_xml_after) {
			zval_ptr_dtor(&map->map_functions.to_xml_after);
		}
		if (map->map_functions.to_zval_before) {
			zval_ptr_dtor(&map->map_functions.to_zval_before);
		}
		if (map->map_functions.to_zval) {
			zval_ptr_dtor(&map->map_functions.to_zval);
		}
		if (map->map_functions.to_zval_after) {
			zval_ptr_dtor(&map->map_functions.to_zval_after);
		}
	}
	efree(map);
}

void delete_type(void *data)
{
	sdlTypePtr type = *((sdlTypePtr*)data);
	if (type->name) {
		free(type->name);
	}
	if (type->namens) {
		free(type->namens);
	}
	if (type->elements) {
		zend_hash_destroy(type->elements);
		free(type->elements);
	}
	if (type->attributes) {
		zend_hash_destroy(type->attributes);
		free(type->attributes);
	}
	if (type->restrictions) {
		delete_restriction_var_int(&type->restrictions->minExclusive);
		delete_restriction_var_int(&type->restrictions->minInclusive);
		delete_restriction_var_int(&type->restrictions->maxExclusive);
		delete_restriction_var_int(&type->restrictions->maxInclusive);
		delete_restriction_var_int(&type->restrictions->totalDigits);
		delete_restriction_var_int(&type->restrictions->fractionDigits);
		delete_restriction_var_int(&type->restrictions->length);
		delete_restriction_var_int(&type->restrictions->minLength);
		delete_restriction_var_int(&type->restrictions->maxLength);
		delete_schema_restriction_var_char(&type->restrictions->whiteSpace);
		delete_schema_restriction_var_char(&type->restrictions->pattern);
		if (type->restrictions->enumeration) {
			zend_hash_destroy(type->restrictions->enumeration);
			free(type->restrictions->enumeration);
		}
		free(type->restrictions);
	}
	free(type);
}

void delete_attribute(void *attribute)
{
	sdlAttributePtr attr = *((sdlAttributePtr*)attribute);

	if (attr->def) {
		free(attr->def);
	}
	if (attr->fixed) {
		free(attr->fixed);
	}
	if (attr->form) {
		free(attr->form);
	}
	if (attr->id) {
		free(attr->id);
	}
	if (attr->name) {
		free(attr->name);
	}
	if (attr->ref) {
		free(attr->ref);
	}
	if (attr->type) {
		free(attr->type);
	}
	if (attr->use) {
		free(attr->use);
	}
	if (attr->extraAttributes) {
		zend_hash_destroy(attr->extraAttributes);
		free(attr->extraAttributes);
	}
}

static void delete_document(void *doc_ptr)
{
	xmlDocPtr doc = *((xmlDocPtr*)doc_ptr);
	xmlFreeDoc(doc);
}


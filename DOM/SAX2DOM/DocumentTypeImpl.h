#ifndef JEZUK_SAX2DOM_DOCUMENTTYPEIMPL_H
#define JEZUK_SAX2DOM_DOCUMENTTYPEIMPL_H

#include <DOM/Simple/DocumentTypeImpl.h>

namespace SAX2DOM
{

template<class stringT, class string_adaptorT>
class DocumentType : public SimpleDOM::DocumentTypeImpl<stringT, string_adaptorT>
{
    typedef SimpleDOM::DocumentTypeImpl<stringT, string_adaptorT> BaseDT;
  public:
    DocumentType(const stringT& qualifiedName,
                 const stringT& publicId,
                 const stringT& systemId) :
      BaseDT(qualifiedName, publicId, systemId)
    {
    } // DocumentType

    virtual ~DocumentType() { }

    //////////////////////////////////////////////////////
    // DOM::DOMImplementation extensions
    void addEntity(SimpleDOM::EntityImpl<stringT, string_adaptorT>* entity)
    {
      BaseDT::addEntity(entity);
    } // addEntity

    void addNotation(SimpleDOM::NotationImpl<stringT, string_adaptorT>* notation)
    {
      BaseDT::addNotation(notation);
    } // addNotation

    void addElement(SimpleDOM::ElementImpl<stringT, string_adaptorT>* element)
    {
      BaseDT::addElement(element);
    } // addElement

    void addDefaultAttr(const stringT& elementName, const stringT& attributeName, const stringT& attributeValue)
    {
      SimpleDOM::ElementImpl<stringT, string_adaptorT>* elem = dynamic_cast<SimpleDOM::ElementImpl<stringT, string_adaptorT>*>(getElements()->getNamedItem(elementName));
      if(elem == 0)
      {
        elem = new SimpleDOM::ElementImpl<stringT, string_adaptorT>(0, elementName);
        addElement(elem);
      }
      SimpleDOM::AttrImpl<stringT, string_adaptorT>* attr = new SimpleDOM::AttrImpl<stringT, string_adaptorT>(ownerDoc_, attributeName);
      attr->setValue(attributeValue);
      attr->setSpecified(false);
      elem->setAttributeNode(attr);
    } // addDefaultAttr

    void addElementId(const stringT& elementName)
    {
      BaseDT::addElementId(elementName);
    } // addElementId
}; // class DOMImplementation

} // namespace SAX2DOM

#endif
// end of file

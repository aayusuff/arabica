#ifndef ARABICA_SAX_MSXML2_H
#define ARABICA_SAX_MSXML2_H
//---------------------------------------------------------------------------
// A SAX2 wrapper class for MSXML component.
//
// $Id$
//
// Changes:
//   21-Jul-2002 Major tweaks to use #include <header> rather then
//               #import <type_library>.  The latter is sensitive to
//               the exact name/version of the library provided by
//               Microsoft and various options provided to the type
//               library reader.  (Found the issue when I attempted
//               to build with MSXML4 rather then MSXML3.)  [kas]
//
//   31-Jul-2002 Created the default PROGID define.  M$. in their
//               wisdom, removed the version independent IDs starting
//               in version 4.0 of the MSXML library.  Saves a lot of
//               'DLL hell' problems but creates others for maintainers.
//               The initialization code will now try the 4.0 ID and
//               then the older (version independant) name.   [kas]
//
//---------------------------------------------------------------------------

#include <SAX/ArabicaConfig.h>
#include <SAX/XMLReader.h>
#include <SAX/InputSource.h>
#include <SAX/SAXParseException.h>
#include <SAX/SAXNotRecognizedException.h>
#include <SAX/SAXNotSupportedException.h>
#include <SAX/ext/LexicalHandler.h>
#include <SAX/ext/DeclHandler.h>
#include <SAX/helpers/PropertyNames.h>
#include <Utils/StringAdaptor.h>
#include <iostream>
#include <Utils/getparam.hpp>

// Include the MSXML definitions.
#include <msxml2.h>

//
// Declare the 'smart pointer' type to simplify COM handling.
#include <comdef.h>
_COM_SMARTPTR_TYPEDEF(ISAXXMLReader, __uuidof(ISAXXMLReader));

namespace SAX
{

struct COMInitializer_tag { };

/**
 * use this as COMInitializer_type if you call 
 * CoInitialize/CoInitializeEx in your own code
 */
class COMExternalInitializer : public COMInitializer_tag
{
  public:
    COMExternalInitializer() { }
    ~COMExternalInitializer() { }
}; // COMExternalInitializer

class COMSingleThreadInitializer : public COMInitializer_tag
{
  public:
    COMSingleThreadInitializer() { ::CoInitialize(NULL);  }
    ~COMSingleThreadInitializer() { ::CoUninitialize(); }
}; // COMSingleThreadInitializer

#if(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)
class COMMultiThreadInitializer  : public COMInitializer_tag
{
  public:
    COMMultiThreadInitializer() { ::CoInitializeEx(NULL, COINIT_MULTITHREADED); } 
    ~COMMultiThreadInitializer() { ::CoUninitialize(); }
};
#endif

template<class string_type, 
         class T0 = Arabica::nil_t,
         class T1 = Arabica::nil_t>
class msxml2_wrapper : public SAX::basic_XMLReader<string_type>
{
  typedef typename Arabica::get_param<Arabica::string_adaptor_tag, 
                               Arabica::default_string_adaptor<string_type>, 
                               T0, 
                               T1>::type string_adaptor_type;
  typedef typename Arabica::get_param<COMInitializer_tag, 
                               COMSingleThreadInitializer, 
                               T1, 
                               T0>::type COMInitializer_type;

  public:
    typedef SAX::basic_EntityResolver<string_type> entityResolverT;
    typedef SAX::basic_DTDHandler<string_type> dtdHandlerT;
    typedef SAX::basic_ContentHandler<string_type> contentHandlerT;
    typedef SAX::basic_DeclHandler<string_type> declHandlerT;
    typedef SAX::basic_LexicalHandler<string_type> lexicalHandlerT;
    typedef SAX::basic_InputSource<string_type> inputSourceT;
    typedef SAX::basic_Locator<string_type> locatorT;
    typedef SAX::basic_ErrorHandler<string_type> errorHandlerT;

    msxml2_wrapper();
    virtual ~msxml2_wrapper();

    /////////////////////////////////////////////////
    // Configuration
    virtual bool getFeature(const string_type& name) const;
    virtual void setFeature(const string_type& name, bool value);
  
    /////////////////////////////////////////////////
    // Event Handlers
    /* MSXML does not use EntityResolver currently */
    virtual void setEntityResolver(SAX::basic_EntityResolver<string_type>& resolver) { } 
    virtual SAX::basic_EntityResolver<string_type>* getEntityResolver() const { return 0; }
    virtual void setDTDHandler(SAX::basic_DTDHandler<string_type>& handler) { dtdHandler_.setDTDHandler(handler); } 
    virtual SAX::basic_DTDHandler<string_type>* getDTDHandler() const { return dtdHandler_.getDTDHandler(); }
    virtual void setContentHandler(SAX::basic_ContentHandler<string_type>& handler) {	contentHandler_.setContentHandler(handler); }
	virtual SAX::basic_ContentHandler<string_type>* getContentHandler() const { return contentHandler_.getContentHandler(); }
    virtual void setErrorHandler(SAX::basic_ErrorHandler<string_type>& handler);
    virtual SAX::basic_ErrorHandler<string_type>* getErrorHandler() const;

    //////////////////////////////////////////////////
    // Parsing
    virtual void parse(SAX::basic_InputSource<string_type>& input);

  protected:
	virtual std::auto_ptr<typename SAX::basic_XMLReader<string_type>::PropertyBase> doGetProperty(const string_type& name);
	virtual void doSetProperty(const string_type& name, std::auto_ptr<typename SAX::basic_XMLReader<string_type>::PropertyBase> value);

	private:
    //////////////////////////////////////////////////////
    // COM interface -> C++ interface adaptors
    class LocatorAdaptor : public SAX::basic_Locator<string_type>
    {
      public:
        LocatorAdaptor() : locator_(0) { }
        LocatorAdaptor(ISAXLocator __RPC_FAR* locator) : locator_(0) { setLocator(locator); } 
        ~LocatorAdaptor() { setLocator(0); }

        string_type getPublicId() const
        {
          if(!locator_) 
            return string_type();

		  const wchar_t* pwchPublicId;
          locator_->getPublicId(&pwchPublicId);
          string_type publicId(string_adaptor_type::construct_from_utf16(pwchPublicId));
          return publicId;
        } // getPublicId
    
        string_type getSystemId() const
        {
          if(!locator_)
            return string_type();

          const wchar_t* pwchSystemId;
          locator_->getSystemId(&pwchSystemId);
          string_type systemId(string_adaptor_type::construct_from_utf16(pwchSystemId));
          return systemId;
        } // getSystemId
    
        int getLineNumber() const
        {
          if(!locator_)
            return -1;

          int lineNumber;
          locator_->getLineNumber(&lineNumber);
          return lineNumber;
        } // getLineNumber
    
		int getColumnNumber() const
        {
          if(!locator_)
            return -1;

          int columnNumber;
          locator_->getColumnNumber(&columnNumber);
          return columnNumber;
        } // getColumnNumber
    
        void setLocator(ISAXLocator __RPC_FAR* locator)
        {
          locator_ = locator;
          // no need to AddRef or Release as the ISAXLocator  points to the MSXML instance
          // and we'll clean that up properly anyway
        } // setLocator

      private:
        ISAXLocator __RPC_FAR *locator_;
    }; // class LocatorAdaptor

    class DTDHandlerAdaptor : public ISAXDTDHandler 
    {
      public:
        DTDHandlerAdaptor() : dtdHandler_(0) { }
        ~DTDHandlerAdaptor() { }

		void setDTDHandler(SAX::basic_DTDHandler<string_type>& handler) { dtdHandler_ = &handler; }
        SAX::basic_DTDHandler<string_type>* getDTDHandler() const { return dtdHandler_; }

    virtual HRESULT STDMETHODCALLTYPE notationDecl( 
            /* [in] */ const wchar_t *pwchName,
            /* [in] */ int cchName,
            /* [in] */ const wchar_t *pwchPublicId,
            /* [in] */ int cchPublicId,
            /* [in] */ const wchar_t *pwchSystemId,
            /* [in] */ int cchSystemId)
        {
          if(dtdHandler_)
            dtdHandler_->notationDecl(string_adaptor_type::construct_from_utf16(pwchName, cchName),
                                      string_adaptor_type::construct_from_utf16(pwchPublicId, cchPublicId),
                                      string_adaptor_type::construct_from_utf16(pwchSystemId, cchSystemId));
          return S_OK;
        } // notationDecl

    virtual HRESULT STDMETHODCALLTYPE unparsedEntityDecl( 
            /* [in] */ const wchar_t *pwchName,
            /* [in] */ int cchName,
            /* [in] */ const wchar_t *pwchPublicId,
            /* [in] */ int cchPublicId,
            /* [in] */ const wchar_t *pwchSystemId,
            /* [in] */ int cchSystemId,
            /* [in] */ const wchar_t *pwchNotationName,
            /* [in] */ int cchNotationName)
	   {
          if(dtdHandler_)
            dtdHandler_->unparsedEntityDecl(string_adaptor_type::construct_from_utf16(pwchName, cchName),
                                            string_adaptor_type::construct_from_utf16(pwchPublicId, cchPublicId),
                                            string_adaptor_type::construct_from_utf16(pwchSystemId, cchSystemId),
                                            string_adaptor_type::construct_from_utf16(pwchNotationName, cchNotationName));
          return S_OK;
        } // unparsedEntityDecl

        // satify COM interface even if we're not a COM object
        long __stdcall QueryInterface(const struct _GUID &riid,void **ppvObject) { return 0; }
        unsigned long __stdcall AddRef() { return 0; }
        unsigned long __stdcall Release() { return 0; }

      private:
        SAX::basic_DTDHandler<string_type>* dtdHandler_;
    }; // class DTDHandlerAdaptor

    class ContentHandlerAdaptor : public ISAXContentHandler 
    {
      public:
        ContentHandlerAdaptor() : contentHandler_(0) { } 
        ~ContentHandlerAdaptor() { }

        void setContentHandler(SAX::basic_ContentHandler<string_type>& handler) { contentHandler_ = &handler; }
        SAX::basic_ContentHandler<string_type>* getContentHandler() const { return contentHandler_; }


        virtual HRESULT STDMETHODCALLTYPE putDocumentLocator( 
                               /* [in] */ ISAXLocator *pLocator)
        {
          locator_.setLocator(pLocator);
          if(contentHandler_) contentHandler_->setDocumentLocator(locator_);
          return S_OK;
        } // putDocumentLocation
        
        virtual HRESULT STDMETHODCALLTYPE startDocument( void)
        {
          if(contentHandler_) contentHandler_->startDocument();
          return S_OK;
        } // startDocument
        
        virtual HRESULT STDMETHODCALLTYPE endDocument( void)
        {
          if(contentHandler_) contentHandler_->endDocument();
          return S_OK;
        } // endDocument
        
        virtual HRESULT STDMETHODCALLTYPE startPrefixMapping( 
                                /* [in] */ const wchar_t *pwchPrefix,
                                /* [in] */ int cchPrefix,
                                /* [in] */ const wchar_t *pwchUri,
                                /* [in] */ int cchUri)

		{
          if(contentHandler_)
            contentHandler_->startPrefixMapping(string_adaptor_type::construct_from_utf16(pwchPrefix, cchPrefix),
                                                string_adaptor_type::construct_from_utf16(pwchUri, cchUri));
          return S_OK;
        } // startPrefixMapping
       
        virtual HRESULT STDMETHODCALLTYPE endPrefixMapping( 
                                /* [in] */ const wchar_t *pwchPrefix,
                                /* [in] */ int cchPrefix)
        {
          if(contentHandler_) contentHandler_->endPrefixMapping(string_adaptor_type::construct_from_utf16(pwchPrefix, cchPrefix));
          return S_OK;
        } // endPrefixMapping
        
        virtual HRESULT STDMETHODCALLTYPE startElement( 
                                /* [in] */ const wchar_t *pwchNamespaceUri,
                                /* [in] */ int cchNamespaceUri,
                                /* [in] */ const wchar_t *pwchLocalName,
                                /* [in] */ int cchLocalName,
                                /* [in] */ const wchar_t *pwchQName,
                                /* [in] */ int cchQName,
                                /* [in] */ ISAXAttributes *pAttributes)
        {
          if(contentHandler_)
          {
            AttributesAdaptor attrs(pAttributes);
			contentHandler_->startElement(string_adaptor_type::construct_from_utf16(pwchNamespaceUri, cchNamespaceUri),
                                          string_adaptor_type::construct_from_utf16(pwchLocalName, cchLocalName), 
                                          string_adaptor_type::construct_from_utf16(pwchQName, cchQName), 
                                          attrs);
          } // if ...
          return S_OK;
        } // startElement
        
        virtual HRESULT STDMETHODCALLTYPE endElement( 
                                /* [in] */ const wchar_t *pwchNamespaceUri,
                                /* [in] */ int cchNamespaceUri,
                                /* [in] */ const wchar_t *pwchLocalName,
                                /* [in] */ int cchLocalName,
                                /* [in] */ const wchar_t *pwchQName,
                                /* [in] */ int cchQName)
        {
          if(contentHandler_)
            contentHandler_->endElement(string_adaptor_type::construct_from_utf16(pwchNamespaceUri, cchNamespaceUri), 
                                        string_adaptor_type::construct_from_utf16(pwchLocalName, cchLocalName), 
                                        string_adaptor_type::construct_from_utf16(pwchQName, cchQName));
          return S_OK;
        } // endElement
        
        virtual HRESULT STDMETHODCALLTYPE characters( 
            /* [in] */ const wchar_t *pwchChars,
            /* [in] */ int cchChars)
        {
		  if(contentHandler_) contentHandler_->characters(string_adaptor_type::construct_from_utf16(pwchChars, cchChars));
          return S_OK;
        } // characters
        
        virtual HRESULT STDMETHODCALLTYPE ignorableWhitespace( 
            /* [in] */ const wchar_t *pwchChars,
            /* [in] */ int cchChars)
        {
          if(contentHandler_)
            contentHandler_->ignorableWhitespace(string_adaptor_type::construct_from_utf16(pwchChars, cchChars));
          return S_OK;
        } // ignorableWhitespace
        
        virtual HRESULT STDMETHODCALLTYPE processingInstruction( 
            /* [in] */ const wchar_t *pwchTarget,
            /* [in] */ int cchTarget,
            /* [in] */ const wchar_t *pwchData,
            /* [in] */ int cchData)
        {
          if(contentHandler_)
            contentHandler_->processingInstruction(string_adaptor_type::construct_from_utf16(pwchTarget, cchTarget),
                                                   string_adaptor_type::construct_from_utf16(pwchData, cchData));
          return S_OK;
        } // processingInstruction
        
        virtual HRESULT STDMETHODCALLTYPE skippedEntity( 
            /* [in] */ const wchar_t *pwchName,
			/* [in] */ int cchName)
        {
          if(contentHandler_)
            contentHandler_->skippedEntity(string_adaptor_type::construct_from_utf16(pwchName, cchName));
          return S_OK;
        } // skippedEntity

        // satisfy COM interface even if we're not a COM object
        long __stdcall QueryInterface(const struct _GUID &riid,void **ppvObject) { return 0; }
        unsigned long __stdcall AddRef() { return 0; }
        unsigned long __stdcall Release() { return 0; }

      private:
        ////////////////////////////////////////////////
        // member varaibles
        SAX::basic_ContentHandler<string_type>* contentHandler_;
        LocatorAdaptor locator_;

        //////////////////////////////////////////////////////
        // COM interface -> C++ interface adaptors
        class AttributesAdaptor : public SAX::basic_Attributes<string_type>
        {
          public:
            AttributesAdaptor(ISAXAttributes __RPC_FAR *pAttributes) : attributes_(pAttributes) { }
            ~AttributesAdaptor() { }

            /////////////////////////
			// indexed access
            virtual int getLength() const
            {
              int length;
              attributes_->getLength(&length);
              return length;
            } // getLength

            virtual string_type getURI(unsigned int index) const
            {
              const wchar_t* pwchUri;
              int cchUri;
              HRESULT hr = attributes_->getURI(index, &pwchUri, &cchUri);
              if (FAILED(hr))
                return string_type();
              string_type uri(string_adaptor_type::construct_from_utf16(pwchUri, cchUri));
              return uri;
            } // getURI

            virtual string_type getLocalName(unsigned int index) const
            {
              const wchar_t* pwchLocalName;
              int cchLocalName;
              HRESULT hr = attributes_->getLocalName(index, &pwchLocalName, &cchLocalName);
              if (FAILED(hr))
                return string_type();
              string_type localName(string_adaptor_type::construct_from_utf16(pwchLocalName, cchLocalName));
			  return localName;
            } // getLocalName

            virtual string_type getQName(unsigned int index) const
            {
              const wchar_t* pwchQName;
              int cchQName;
              HRESULT hr = attributes_->getQName(index, &pwchQName, &cchQName);
              if (FAILED(hr))
                return string_type();
              string_type qName(string_adaptor_type::construct_from_utf16(pwchQName, cchQName));
              return qName;
            } // getQName

            virtual string_type getType(unsigned int index) const
            {
              const wchar_t* pwchType;
              int cchType;
              HRESULT hr = attributes_->getType(index, &pwchType, &cchType);
              if (FAILED(hr))
                return string_type();
              string_type type(string_adaptor_type::construct_from_utf16(pwchType, cchType));
              return type;
            } // getType

            virtual string_type getValue(unsigned int index) const
            {
			  const wchar_t* pwchValue;
              int cchValue;
              HRESULT hr = attributes_->getValue(index, &pwchValue, &cchValue);
              if (FAILED(hr))
                return string_type();
              string_type value(string_adaptor_type::construct_from_utf16(pwchValue, cchValue));
              return value;
            } // getValue

            /////////////////////////
            // name based query
            virtual int getIndex(const string_type& uri, const string_type& localName) const
            {
              int index = -1;
              std::wstring wUri(string_adaptor_type::asStdWString(uri));
              std::wstring wLocalName(string_adaptor_type::asStdWString(localName));
              HRESULT hr = attributes_->getIndexFromName(wUri.data(), static_cast<int>(wUri.length()),
                                                         wLocalName.data(), static_cast<int>(wLocalName.length()),
                                                         &index);
              return index;
            } // getIndex

            virtual int getIndex(const string_type& qName) const
            {
              int index = -1;
              std::wstring wQName(string_adaptor_type::asStdWString(qName));
              attributes_->getIndexFromQName(wQName.data(), static_cast<int>(wQName.length()), &index);
			  return index;
            } // getIndex

            virtual string_type getType(const string_type& uri, const string_type& localName) const
            {
              const wchar_t* pwchType;
              int cchType;
              std::wstring wUri(string_adaptor_type::asStdWString(uri));
              std::wstring wLocalName(string_adaptor_type::asStdWString(localName));
              HRESULT hr = attributes_->getTypeFromName(wUri.data(), static_cast<int>(wUri.length()),
                                                        wLocalName.data(), static_cast<int>(wLocalName.length()),
                                                        &pwchType, &cchType);
              if (FAILED(hr))
                return string_type();
              string_type type(string_adaptor_type::construct_from_utf16(pwchType, cchType));
              return type;
            } // getType

            virtual string_type getType(const string_type& qName) const
            {
              const wchar_t* pwchType;
              int cchType;
              std::wstring wQName(string_adaptor_type::asStdWString(qName));
              HRESULT hr = attributes_->getTypeFromQName(wQName.data(), static_cast<int>(wQName.length()),
                                                         &pwchType, &cchType);
              if (FAILED(hr))
                return string_type();
			  string_type type(string_adaptor_type::construct_from_utf16(pwchType, cchType));
              return type;
            } // getType

            virtual string_type getValue(const string_type& uri, const string_type& localName) const
            {
              const wchar_t* pwchValue;
              int cchValue;
              std::wstring wUri(string_adaptor_type::asStdWString(uri));
              std::wstring wLocalName(string_adaptor_type::asStdWString(localName));
              HRESULT hr = attributes_->getValueFromName(wUri.data(), static_cast<int>(wUri.length()),
                                                         wLocalName.data(), static_cast<int>(wLocalName.length()),
                                                         &pwchValue, &cchValue);
              if (FAILED(hr))
                return string_type();
              string_type value(string_adaptor_type::construct_from_utf16(pwchValue, cchValue));
              return value;
            } // getValue

            virtual string_type getValue(const string_type& qname) const
            {
              const wchar_t* pwchValue;
              int cchValue;
              std::wstring wQName(string_adaptor_type::asStdWString(qname));
              HRESULT hr = attributes_->getValueFromQName(wQName.data(), static_cast<int>(wQName.length()),
                                                          &pwchValue, &cchValue);
              if (FAILED(hr))
				return string_type();
              string_type value(string_adaptor_type::construct_from_utf16(pwchValue, cchValue));
              return value;
            } // getValue

          private:
            ISAXAttributes __RPC_FAR *attributes_;

            AttributesAdaptor();
        }; // class AttributesAdaptor
    }; // class ContentHandlerAdaptor

    class ErrorHandlerAdaptor : public ISAXErrorHandler 
    {
      public:
        ErrorHandlerAdaptor() : errorHandler_(0), 
                                bWarning_(false), bError_(false), bFatal_(false),
                                eWarning_("none"), eError_("none"), eFatal_("none")
                                { }
        virtual ~ErrorHandlerAdaptor() { }

        void setErrorHandler(SAX::basic_ErrorHandler<string_type>& handler) { errorHandler_ = &handler; }
        SAX::basic_ErrorHandler<string_type>* getErrorHandler() const { return errorHandler_; }

        virtual HRESULT STDMETHODCALLTYPE error( 
            /* [in] */ ISAXLocator *pLocator,
            /* [in] */ const wchar_t *pwchErrorMessage,
			/* [in] */ HRESULT hrErrorCode)
        {
          bError_ = true;
          string_type errorMsg(string_adaptor_type::construct_from_utf16(pwchErrorMessage));
          eError_ = SAXParseExceptionT(string_adaptor_type::asStdString(errorMsg), LocatorAdaptor(pLocator));
          return S_OK;
        } // error

        virtual HRESULT STDMETHODCALLTYPE fatalError( 
            /* [in] */ ISAXLocator *pLocator,
            /* [in] */ const wchar_t *pwchErrorMessage,
            /* [in] */ HRESULT hrErrorCode)
        {
          bFatal_ = true;
          string_type errorMsg(string_adaptor_type::construct_from_utf16(pwchErrorMessage));
          eFatal_ = SAXParseExceptionT(string_adaptor_type::asStdString(errorMsg), LocatorAdaptor(pLocator));
          return S_FALSE;
        } // fatalError

        virtual HRESULT STDMETHODCALLTYPE ignorableWarning( 
            /* [in] */ ISAXLocator *pLocator,
            /* [in] */ const wchar_t *pwchErrorMessage,
            /* [in] */ HRESULT hrErrorCode)
        {
          bWarning_ = true;
          string_type errorMsg(string_adaptor_type::construct_from_utf16(pwchErrorMessage));
          eWarning_ = SAXParseExceptionT(string_adaptor_type::asStdString(errorMsg), LocatorAdaptor(pLocator));
		  return S_OK;
        } // ignorableWarning

        void report()
        {
          if(!errorHandler_)
            return;

          bool bWarning = bWarning_;
          bool bError = bError_;
          bool bFatal = bFatal_;

          bWarning_ = bError_ = bFatal_ = false;

          if(bFatal)
            errorHandler_->fatalError(eFatal_);
          if(bError)
            errorHandler_->error(eError_);
          if(bWarning)
            errorHandler_->warning(eWarning_);
        } // report

        // satisfy COM interface even if we're not a COM object
        long __stdcall QueryInterface(const struct _GUID &riid,void **ppvObject) { return 0; }
        unsigned long __stdcall AddRef() { return 0; }
        unsigned long __stdcall Release() { return 0; }

	  private:
        typedef SAX::basic_SAXParseException<string_type> SAXParseExceptionT;
        bool bWarning_;
        bool bError_;
        bool bFatal_;
        SAXParseExceptionT eWarning_;
        SAXParseExceptionT eError_;
        SAXParseExceptionT eFatal_;

        SAX::basic_ErrorHandler<string_type>* errorHandler_;
    }; // class ErrorHandlerAdaptor

    class LexicalHandlerAdaptor : public ISAXLexicalHandler 
    {
      public:
        LexicalHandlerAdaptor() : lexicalHandler_(0) { }
        virtual ~LexicalHandlerAdaptor() { }

        void setLexicalHandler(SAX::basic_LexicalHandler<string_type>& handler) { lexicalHandler_ = &handler; }
        SAX::basic_LexicalHandler<string_type>* getLexicalHandler() const { return lexicalHandler_; }

        virtual HRESULT STDMETHODCALLTYPE startDTD( 
            /* [in] */ const wchar_t *pwchName,
            /* [in] */ int cchName,
            /* [in] */ const wchar_t *pwchPublicId,
            /* [in] */ int cchPublicId,
            /* [in] */ const wchar_t *pwchSystemId,
			/* [in] */ int cchSystemId)
        {
          if(lexicalHandler_)
            lexicalHandler_->startDTD(string_adaptor_type::construct_from_utf16(pwchName, cchName),
                                      string_adaptor_type::construct_from_utf16(pwchPublicId, cchPublicId),
                                      string_adaptor_type::construct_from_utf16(pwchSystemId, cchSystemId));
          return S_OK;
        } // startDTD

        virtual HRESULT STDMETHODCALLTYPE endDTD( void)
        {
          if(lexicalHandler_)
            lexicalHandler_->endDTD();
          return S_OK;
        } // endDTD

        virtual HRESULT STDMETHODCALLTYPE startEntity( 
            /* [in] */ const wchar_t *pwchName,
            /* [in] */ int cchName)
        {
          if(lexicalHandler_)
            lexicalHandler_->startEntity(string_adaptor_type::construct_from_utf16(pwchName, cchName));
          return S_OK;
        } // startEntity

        virtual HRESULT STDMETHODCALLTYPE endEntity( 
            /* [in] */ const wchar_t *pwchName,
			/* [in] */ int cchName)
        {
          if(lexicalHandler_)
            lexicalHandler_->endEntity(string_adaptor_type::construct_from_utf16(pwchName, cchName));
          return S_OK;
        } // endEntity

        virtual HRESULT __stdcall startCDATA()
        {
          if(lexicalHandler_)
            lexicalHandler_->startCDATA();
          return S_OK;
        } // startCDATA

        virtual HRESULT __stdcall endCDATA()
        {
          if(lexicalHandler_)
            lexicalHandler_->endCDATA();
          return S_OK;
        } // endCDATA

        virtual HRESULT STDMETHODCALLTYPE comment( 
            /* [in] */ const wchar_t *pwchChars,
            /* [in] */ int cchChars)
        {
          if(lexicalHandler_)
            lexicalHandler_->comment(string_adaptor_type::construct_from_utf16(pwchChars, cchChars));
		  return S_OK;
        } // comment

        // satisfy COM interface even if we're not a COM object
        long __stdcall QueryInterface(const struct _GUID &riid,void **ppvObject)
        {
          // we have to implement this, because we pass this as an IUnknown but it needs an
          // ISAXLexicalHandler interface
          if(riid == __uuidof(ISAXLexicalHandler))
            *ppvObject = this;
          return 0;
        } // QueryInterface
        unsigned long __stdcall AddRef() { return 0; }
        unsigned long __stdcall Release() { return 0; }

      private:
        SAX::basic_LexicalHandler<string_type>* lexicalHandler_;
    }; // class LexicalHandlerAdaptor

    class DeclHandlerAdaptor : public ISAXDeclHandler 
    {
      public:
        DeclHandlerAdaptor() : declHandler_(0) { }
        virtual ~DeclHandlerAdaptor() { }

        void setDeclHandler(SAX::basic_DeclHandler<string_type>& handler) { declHandler_ = &handler; }
        SAX::basic_DeclHandler<string_type>* getDeclHandler() const { return declHandler_; }

        virtual HRESULT STDMETHODCALLTYPE elementDecl( 
            /* [in] */ const wchar_t *pwchName,
            /* [in] */ int cchName,
            /* [in] */ const wchar_t *pwchModel,
            /* [in] */ int cchModel)
        {
          if(declHandler_)
            declHandler_->elementDecl(string_adaptor_type::construct_from_utf16(pwchName, cchName),
                                      string_adaptor_type::construct_from_utf16(pwchModel, cchModel));
          return S_OK;
        } // elementDecl

        virtual HRESULT STDMETHODCALLTYPE attributeDecl( 
            /* [in] */ const wchar_t *pwchElementName,
            /* [in] */ int cchElementName,
            /* [in] */ const wchar_t *pwchAttributeName,
            /* [in] */ int cchAttributeName,
            /* [in] */ const wchar_t *pwchType,
            /* [in] */ int cchType,
            /* [in] */ const wchar_t *pwchValueDefault,
            /* [in] */ int cchValueDefault,
            /* [in] */ const wchar_t *pwchValue,
            /* [in] */ int cchValue)
        {
          if(declHandler_)
            declHandler_->attributeDecl(string_adaptor_type::construct_from_utf16(pwchElementName, cchElementName),
										string_adaptor_type::construct_from_utf16(pwchAttributeName, cchAttributeName),
                                        string_adaptor_type::construct_from_utf16(pwchType, cchType),
                                        string_adaptor_type::construct_from_utf16(pwchValueDefault, cchValueDefault),
                                        string_adaptor_type::construct_from_utf16(pwchValue, cchValue));
          return S_OK;
        } // attributeDecl

        virtual HRESULT STDMETHODCALLTYPE internalEntityDecl( 
            /* [in] */ const wchar_t *pwchName,
            /* [in] */ int cchName,
            /* [in] */ const wchar_t *pwchValue,
            /* [in] */ int cchValue)
        {
          if(declHandler_)
            declHandler_->internalEntityDecl(string_adaptor_type::construct_from_utf16(pwchName, cchName),
                                             string_adaptor_type::construct_from_utf16(pwchValue, cchValue));
          return S_OK;
        } // internalEntityDecl

        virtual HRESULT STDMETHODCALLTYPE externalEntityDecl( 
            /* [in] */ const wchar_t *pwchName,
            /* [in] */ int cchName,
            /* [in] */ const wchar_t *pwchPublicId,
            /* [in] */ int cchPublicId,
            /* [in] */ const wchar_t *pwchSystemId,
            /* [in] */ int cchSystemId)
        {
		  if(declHandler_)
            declHandler_->externalEntityDecl(string_adaptor_type::construct_from_utf16(pwchName, cchName),
                                             string_adaptor_type::construct_from_utf16(pwchPublicId, cchPublicId),
                                             string_adaptor_type::construct_from_utf16(pwchSystemId, cchSystemId));
          return S_OK;
        } // externalEntityDecl

        // satisfy COM interface even if we're not a COM object
        long __stdcall QueryInterface(const struct _GUID &riid,void **ppvObject)
        {
          // we have to implement this, because we pass this as an IUnknown but it needs an
          // ISAXDeclHandler interface
          if(riid == __uuidof(ISAXDeclHandler))
            *ppvObject = reinterpret_cast<void*>(this);
          return 0;
        } // QueryInterface

        unsigned long __stdcall AddRef() { return 0; }
        unsigned long __stdcall Release() { return 0; }

      private:
        SAX::basic_DeclHandler<string_type>* declHandler_;
    }; // class DeclHandlerAdaptor

    class StreamAdaptor : public ISequentialStream
    {
      public:
		StreamAdaptor(SAX::basic_InputSource<string_type>& source) :
          source_(source)
        {
        } // StreamAdaptor

        virtual HRESULT __stdcall Read(void* pv, ULONG cb, ULONG* pcbRead)
        {
          source_.getByteStream()->read(reinterpret_cast<char*>(pv), cb);
          *pcbRead = source_.getByteStream()->gcount();
          return S_OK;
        } // Read

        virtual HRESULT __stdcall Write(const void __RPC_FAR *pv, ULONG cb, ULONG __RPC_FAR *pcbWritten)
        {
          return S_FALSE;
        } // Write

        // satisfy COM interface even if we're not a COM object
        long __stdcall QueryInterface(const struct _GUID &riid,void **ppvObject)
        {
          // we have to implement this, because we pass this as an IUnknown but it needs an
          // IStream interface
          if(riid == __uuidof(ISequentialStream))
          {
            *ppvObject = reinterpret_cast<void*>(this);
            return S_OK;
          } // if ...
		  return E_NOINTERFACE;
        } // QueryInterface

        unsigned long __stdcall AddRef() { return 1; }
        unsigned long __stdcall Release() { return 1; }

      private:
        SAX::basic_InputSource<string_type>& source_;
    }; // StreamAdaptor


    //////////////////////////////////////////////////////
    // member variables
    COMInitializer_type init;
    DTDHandlerAdaptor dtdHandler_;
    ContentHandlerAdaptor contentHandler_;
    ErrorHandlerAdaptor errorHandler_;
    LexicalHandlerAdaptor lexicalHandler_;
    DeclHandlerAdaptor declHandler_;

    ISAXXMLReaderPtr reader_;
    SAX::PropertyNames<string_type, string_adaptor_type> properties_;
}; // class msxml

template<class string_type, class T0, class T1>
msxml2_wrapper<string_type, T0, T1>::msxml2_wrapper()
{
  reader_.CreateInstance("Msxml2.SAXXMLReader.4.0");
  if(reader_.GetInterfacePtr() == 0)
    reader_.CreateInstance("Msxml2.SAXXMLReader.3.0");
  if(reader_.GetInterfacePtr() == 0)
    reader_.CreateInstance(__uuidof(ISAXXMLReader));

  if(reader_.GetInterfacePtr() == 0)
    throw SAXException("MSXML SAX Reader (pre-4.0) could not be instantiated");

  reader_->putContentHandler(&contentHandler_);
  reader_->putErrorHandler(&errorHandler_);
  reader_->putDTDHandler(&dtdHandler_);

  VARIANT wrapper;
  wrapper.vt = VT_UNKNOWN;
  wrapper.punkVal = static_cast<ISAXLexicalHandler*>(&lexicalHandler_);
  reader_->putProperty(L"http://xml.org/sax/properties/lexical-handler", wrapper);
  wrapper.punkVal = static_cast<ISAXDeclHandler*>(&declHandler_);
  reader_->putProperty(L"http://xml.org/sax/properties/declaration-handler", wrapper);
} // msxml2_wrapper

template<class string_type, class T0, class T1>
msxml2_wrapper<string_type, T0, T1>::~msxml2_wrapper()
{
  if(reader_.GetInterfacePtr())
    reader_.Release();
} // ~msxml2_wrapper

template<class string_type, class T0, class T1>
bool msxml2_wrapper<string_type, T0, T1>::getFeature(const string_type& name) const
{
  VARIANT_BOOL feature;
  std::wstring wName(string_adaptor_type::asStdWString(name));
  reader_->getFeature(wName.c_str(), &feature);
  return (feature == VARIANT_TRUE) ? true : false;
} // msxml2_wrapper::getFeature

template<class string_type, class T0, class T1>
void msxml2_wrapper<string_type, T0, T1>::setFeature(const string_type& name, bool value)
{
  std::wstring wName(string_adaptor_type::asStdWString(name));
  reader_->putFeature(wName.c_str(), value);
} // setFeature

template<class string_type, class T0, class T1>
void msxml2_wrapper<string_type, T0, T1>::setErrorHandler(SAX::basic_ErrorHandler<string_type>& handler)
{
	errorHandler_.setErrorHandler(handler);
} // setErrorHandler

template<class string_type, class T0, class T1>
SAX::basic_ErrorHandler<string_type>* msxml2_wrapper<string_type, T0, T1>::getErrorHandler() const
{
  return errorHandler_.getErrorHandler();
} // getErrorHandler

template<class string_type, class T0, class T1>
#ifndef ARABICA_VS6_WORKAROUND
std::auto_ptr<typename SAX::basic_XMLReader<string_type>::PropertyBase> msxml2_wrapper<string_type, T0, T1>::doGetProperty(const string_type& name)
#else
std::auto_ptr<SAX::basic_XMLReader<string_type>::PropertyBase> msxml2_wrapper<string_type, T0, T1>::doGetProperty(const string_type& name)
#endif
{
  if(name == properties_.lexicalHandler)
  {
    Property<SAX::basic_LexicalHandler<string_type>*>* prop = new Property<SAX::basic_LexicalHandler<string_type>*>(lexicalHandler_.getLexicalHandler());
    return std::auto_ptr<SAX::basic_XMLReader<string_type>::PropertyBase>(prop);
  }
  if(name == properties_.declHandler)
  {
    Property<SAX::basic_DeclHandler<string_type>*>* prop = new Property<SAX::basic_DeclHandler<string_type>*>(declHandler_.getDeclHandler());
    return std::auto_ptr<SAX::basic_XMLReader<string_type>::PropertyBase>(prop);
  }
  throw SAX::SAXNotRecognizedException("Property not recognized ");    
} // doGetProperty

template<class string_type, class T0, class T1>
void msxml2_wrapper<string_type, T0, T1>::doSetProperty(const string_type& name, std::auto_ptr<typename SAX::basic_XMLReader<string_type>::PropertyBase> value)
{
  if(name == properties_.lexicalHandler)
  {
	Property<SAX::basic_LexicalHandler<string_type>&>* prop = dynamic_cast<Property<SAX::basic_LexicalHandler<string_type>&>*>(value.get());

    if(!prop)
      throw std::runtime_error("bad_cast: Property LexicalHandler is wrong type, should be SAX::LexicalHandler&");

    lexicalHandler_.setLexicalHandler(prop->get());
    return;
  } // if ...
  if(name == properties_.declHandler)
  {
    Property<SAX::basic_DeclHandler<string_type>&>* prop = dynamic_cast<Property<SAX::basic_DeclHandler<string_type>&>*>(value.get());

    if(!prop)
      throw std::runtime_error("bad_cast: Property DeclHandler is wrong type, should be SAX::DeclHandler&");

    declHandler_.setDeclHandler(prop->get());
    return;
  } // if ...
  throw SAX::SAXNotRecognizedException("Property not recognized ");    
} // doSetProperty

template<class string_type, class T0, class T1>
void msxml2_wrapper<string_type, T0, T1>::parse(SAX::basic_InputSource<string_type>& source)
{
  if(source.getByteStream() == 0)
  {
    std::wstring wSysId(string_adaptor_type::asStdWString(source.getSystemId()));
    reader_->parseURL(wSysId.c_str());
  }
  else
  {
    StreamAdaptor sa(source);
    VARIANT wrapper;
    wrapper.vt = VT_UNKNOWN;
    wrapper.punkVal = static_cast<ISequentialStream*>(&sa);
    reader_->parse(wrapper);
  } // if ...
  errorHandler_.report();
} // parse

} // namespace SAX

#endif
// end of file

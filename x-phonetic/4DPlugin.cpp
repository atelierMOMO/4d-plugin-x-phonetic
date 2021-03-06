/* --------------------------------------------------------------------------------
 #
 #	4DPlugin.cpp
 #	source generated by 4D Plugin Wizard
 #	Project : phonetic
 #	author : miyako
 #	2016/08/02
 #
 # --------------------------------------------------------------------------------*/


#include "4DPluginAPI.h"
#include "4DPlugin.h"

#pragma mark -

bool IsProcessOnExit()
{
	C_TEXT name;
	PA_long32 state, time;
	PA_GetProcessInfo(PA_GetCurrentProcessNumber(), name, &state, &time);
	CUTF16String procName(name.getUTF16StringPtr());
	CUTF16String exitProcName((PA_Unichar *)"$\0x\0x\0\0\0");
	return (!procName.compare(exitProcName));
}

#if VERSIONWIN
void CoInit()
{
	setlocale(LC_ALL, "Japanese");
	OleInitialize(NULL);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
}
void CoDeinit()
{
	CoUninitialize();
}

#endif

void OnStartup()
{
#if VERSIONWIN
	PA_RunInMainProcess((PA_RunInMainProcessProcPtr)CoInit, NULL);
#endif
}

void OnCloseProcess()
{
#if VERSIONWIN
	if(IsProcessOnExit())
	{
		PA_RunInMainProcess((PA_RunInMainProcessProcPtr)CoDeinit, NULL);
	}
#endif
}

#pragma mark -

void PluginMain(PA_long32 selector, PA_PluginParameters params)
{
	try
	{
		PA_long32 pProcNum = selector;
		sLONG_PTR *pResult = (sLONG_PTR *)params->fResult;
		PackagePtr pParams = (PackagePtr)params->fParameters;

		CommandDispatcher(pProcNum, pResult, pParams); 
	}
	catch(...)
	{

	}
}

void CommandDispatcher (PA_long32 pProcNum, sLONG_PTR *pResult, PackagePtr pParams)
{
	
	switch(pProcNum)
	{
// --- phonetic

		case 1 :
			Phonetic(pResult, pParams);
			break;

	}
}

// ----------------------------------- phonetic -----------------------------------


void Phonetic(sLONG_PTR *pResult, PackagePtr pParams)
{
	C_TEXT Param1;
	C_TEXT returnValue;

	Param1.fromParamAtIndex(pParams, 1);

	// --- write the code of Phonetic here...
	
#if VERSIONMAC
	NSString *sourceText = Param1.copyUTF16String();
	NSMutableString *outputTextHiragana = [[NSMutableString alloc]init];
	CFLocaleRef locale = CFLocaleCopyCurrent();
	CFRange range = CFRangeMake(0, [sourceText length]);
	
	CFStringTokenizerRef tokenizer = CFStringTokenizerCreate(kCFAllocatorDefault,
																													 (CFStringRef)sourceText,
																													 range,
																													 kCFStringTokenizerUnitWordBoundary,
																													 locale);
	CFStringTokenizerTokenType tokenType = CFStringTokenizerGoToTokenAtIndex(tokenizer, 0);
	while (tokenType != kCFStringTokenizerTokenNone)
	{
		range = CFStringTokenizerGetCurrentTokenRange(tokenizer);
		CFTypeRef latinTranscription = CFStringTokenizerCopyCurrentTokenAttribute(tokenizer, kCFStringTokenizerAttributeLatinTranscription);
		if(latinTranscription)
		{
			NSMutableString *hiragana = [(NSString *)latinTranscription mutableCopy];
			CFStringTransform((CFMutableStringRef)hiragana, NULL, kCFStringTransformLatinHiragana, false);
			CFRelease(latinTranscription);
			[outputTextHiragana appendString:hiragana];
		}
		tokenType = CFStringTokenizerAdvanceToNextToken(tokenizer);
	}
	CFRelease(tokenizer);
	CFRelease(locale);
	
	returnValue.setUTF16String(outputTextHiragana);
	[outputTextHiragana release];
	[sourceText release];
	
#else
	
	IFELanguage *ifelang = NULL;
	
	CLSID clsid;
	static const IID IID_IFELanguage = {
        0x019f7152, 0xe6db, 0x11d0,
        { 0x83, 0xc3, 0x00, 0xc0, 0x4f, 0xdd, 0xb8, 0x2e }
    };
	if(S_OK == CLSIDFromProgID(OLESTR("MSIME.Japan"), &clsid))
	{
		if(S_OK == CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IFELanguage, reinterpret_cast<void**>(&ifelang)))
		{
			if(ifelang)
			{
				if(S_OK == ifelang->Open())
				{
					BSTR sourceText = SysAllocString((const wchar_t *)Param1.getUTF16StringPtr());
					BSTR phonetic;
					ifelang->GetPhonetic(sourceText, 1, -1, &phonetic);
					returnValue.setUTF16String((const PA_Unichar *)phonetic, SysStringLen(phonetic));
					SysFreeString(sourceText);
					SysFreeString(phonetic);
				}
				ifelang->Close();
				ifelang->Release();
			}
		}
	}
#endif

	returnValue.setReturn(pResult);
}


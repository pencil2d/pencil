#include <windows.h>
#include <gdiplus.h>

#include <CommCtrl.h>
#include <Msi.h>

#include "dutil.h"
#include "dictutil.h"
#include "locutil.h"
#include "pathutil.h"
#include "strutil.h"
#include "thmutil.h"
#include "xmlutil.h"

#include "BootstrapperEngine.h"
#include "BootstrapperApplication.h"
#include "BAFunctions.h"

#include "IBootstrapperEngine.h"
#include "IBootstrapperApplication.h"
#include "IBAFunctions.h"

#include "BalBaseBAFunctions.h"
#include "BalBaseBAFunctionsProc.h"

#include "balutil.h"

enum PENCIL2DBAFUNCTIONS_CONTROL
{
    PENCIL2DBAFUNCTIONS_PROGRESS_ACTION_TEXT = THEME_FIRST_ASSIGN_CONTROL_ID,

    LAST_PENCIL2DBAFUNCTIONS_CONTROL,
};

class Pencil2DBAFunctions : public CBalBaseBAFunctions
{
public:
    Pencil2DBAFunctions(
        __in HMODULE hModule,
        __in IBootstrapperEngine* pEngine,
        __in const BA_FUNCTIONS_CREATE_ARGS* pArgs,
        __in WIX_LOCALIZATION* pWixLoc
        ) : CBalBaseBAFunctions(hModule, pEngine, pArgs), m_pWixLoc(pWixLoc)
    {
    }

    ~Pencil2DBAFunctions()
    {
        LocFree(m_pWixLoc);
    }

    virtual STDMETHODIMP OnThemeControlLoading(
        __in LPCWSTR wzName,
        __inout BOOL* pfProcessed,
        __inout WORD* pwId,
        __inout DWORD* pdwAutomaticBehaviorType
        )
    {
        if (::CompareStringW(LOCALE_NEUTRAL, 0, wzName, -1, L"ProgressActionText", -1) == CSTR_EQUAL)
        {
            *pfProcessed = TRUE;
            *pwId = PENCIL2DBAFUNCTIONS_PROGRESS_ACTION_TEXT;
            *pdwAutomaticBehaviorType = THEME_CONTROL_AUTOMATIC_BEHAVIOR_EXCLUDE_ENABLED | THEME_CONTROL_AUTOMATIC_BEHAVIOR_EXCLUDE_VISIBLE | THEME_CONTROL_AUTOMATIC_BEHAVIOR_EXCLUDE_ACTION | THEME_CONTROL_AUTOMATIC_BEHAVIOR_EXCLUDE_VALUE;
            return S_OK;
        }

        return __super::OnThemeControlLoading(wzName, pfProcessed, pwId, pdwAutomaticBehaviorType);
    }

    virtual STDMETHODIMP OnThemeControlLoaded(
        __in LPCWSTR wzName,
        __in WORD wId,
        __in HWND hWnd,
        __inout BOOL* pfProcessed
    )
    {
        HRESULT hr = S_OK;

        WCHAR szClass[countof(PROGRESS_CLASSW)] = L"";
        if (!::GetClassNameW(hWnd, szClass, countof(szClass)))
        {
            BalExitOnLastError(hr, "Failed to get window class.");
        }

        if (::CompareStringW(LOCALE_NEUTRAL, 0, szClass, -1, PROGRESS_CLASSW, -1) == CSTR_EQUAL &&
            ::GetWindowLongPtrW(hWnd, GWL_STYLE) & PBS_MARQUEE)
        {
            ::SendMessageW(hWnd, PBM_SETMARQUEE, TRUE, 0);
        }

        if (wId == PENCIL2DBAFUNCTIONS_PROGRESS_ACTION_TEXT)
        {
            if (m_hwndControlProgressActionText)
            {
                BalLog(BOOTSTRAPPER_LOG_LEVEL_ERROR, "Duplicate control name: %ls", wzName);
            }
            else
            {
                m_hwndControlProgressActionText = hWnd;
            }

            *pfProcessed = TRUE;
            ExitFunction();
        }

        hr =  __super::OnThemeControlLoaded(wzName, wId, hWnd, pfProcessed);

    LExit:
        return hr;
    }

    virtual STDMETHODIMP OnDetectRelatedBundle(
        __in_z LPCWSTR wzBundleId,
        __in BOOTSTRAPPER_RELATION_TYPE relationType,
        __in_z LPCWSTR wzBundleTag,
        __in BOOL fPerMachine,
        __in_z LPCWSTR wzVersion,
        __in BOOL fMissingFromCache,
        __inout BOOL* pfCancel
        )
    {
        LPWSTR wzValue = NULL;
        LONGLONG llValue = 0;

        BalLog(BOOTSTRAPPER_LOG_LEVEL_STANDARD, "Trying to recover installation options from related bundle %ls.", wzBundleId);
        if (SUCCEEDED(BalGetRelatedBundleVariable(wzBundleId, L"InstallFolder", &wzValue)) && wzValue)
        {
            BalSetStringVariable(L"InstallFolder", wzValue, TRUE);
            BalLog(BOOTSTRAPPER_LOG_LEVEL_STANDARD, "Recovered install folder: %ls.", wzValue);
        }

        if (SUCCEEDED(BalGetRelatedBundleVariable(wzBundleId, L"DesktopShortcut", &wzValue)) && wzValue)
        {
            if (SUCCEEDED(StrStringToInt64(wzValue, 0, &llValue)))
            {
                BalSetNumericVariable(L"DesktopShortcut", llValue);
                BalLog(BOOTSTRAPPER_LOG_LEVEL_STANDARD, "Recovered desktop shortcut option: %lld.", llValue);
            }
            else
            {
                BalLog(BOOTSTRAPPER_LOG_LEVEL_STANDARD, "Failed to convert desktop shortcut option '%ls' from related bundle to number, ignoring.", wzValue);
            }
        }

        return __super::OnDetectRelatedBundle(wzBundleId, relationType, wzBundleTag, fPerMachine, wzVersion, fMissingFromCache, pfCancel);
    }

    virtual STDMETHODIMP OnPauseAutomaticUpdatesBegin()
    {
        if (m_hwndControlProgressActionText) {
            LOC_STRING* pLocString = NULL;
            LocGetString(m_pWixLoc, L"#(loc.PauseAutomaticUpdatesMessage)", &pLocString);
            LPWSTR sczFormattedString = NULL;
            if (pLocString)
            {
                BalFormatString(pLocString->wzText, &sczFormattedString);
            }
            ::SetWindowTextW(m_hwndControlProgressActionText, sczFormattedString ? sczFormattedString : L"Pausing Windows automatic updates");
        }

        return __super::OnPauseAutomaticUpdatesBegin();
    }

    virtual STDMETHODIMP OnSystemRestorePointBegin()
    {
        if (m_hwndControlProgressActionText) {
            LOC_STRING* pLocString = NULL;
            LocGetString(m_pWixLoc, L"#(loc.SystemRestorePointMessage)", &pLocString);
            LPWSTR sczFormattedString = NULL;
            if (pLocString)
            {
                BalFormatString(pLocString->wzText, &sczFormattedString);
            }
            ::SetWindowTextW(m_hwndControlProgressActionText, sczFormattedString ? sczFormattedString : L"Creating system restore point");
        }

        return __super::OnSystemRestorePointBegin();
    }

    virtual STDMETHODIMP OnCacheBegin(
        __inout BOOL* pfCancel
        )
    {
        if (m_hwndControlProgressActionText) {
            LOC_STRING* pLocString = NULL;
            LocGetString(m_pWixLoc, L"#(loc.CacheMessage)", &pLocString);
            LPWSTR sczFormattedString = NULL;
            if (pLocString)
            {
                BalFormatString(pLocString->wzText, &sczFormattedString);
            }
            ::SetWindowTextW(m_hwndControlProgressActionText, sczFormattedString ? sczFormattedString : L"Preparing files");
        }

        return __super::OnCacheBegin(pfCancel);
    }

    virtual STDMETHODIMP OnExecutePackageBegin(
        __in_z LPCWSTR wzPackageId,
        __in BOOL fExecute,
        __in BOOTSTRAPPER_ACTION_STATE action,
        __in INSTALLUILEVEL uiLevel,
        __in BOOL fDisableExternalUiHandler,
        __inout BOOL* pfCancel
        )
    {
        if (m_hwndControlProgressActionText) {
            LOC_STRING* pLocString = NULL;
            switch (action) {
                case BOOTSTRAPPER_ACTION_STATE_UNINSTALL:
                    LocGetString(m_pWixLoc, L"#(loc.ExecuteUninstallPackagesMessage)", &pLocString);
                    break;
                case BOOTSTRAPPER_ACTION_STATE_INSTALL:
                    LocGetString(m_pWixLoc, L"#(loc.ExecuteInstallPackagesMessage)", &pLocString);
                    break;
                case BOOTSTRAPPER_ACTION_STATE_MODIFY:
                    LocGetString(m_pWixLoc, L"#(loc.ExecuteModifyPackagesMessage)", &pLocString);
                    break;
                case BOOTSTRAPPER_ACTION_STATE_REPAIR:
                    LocGetString(m_pWixLoc, L"#(loc.ExecuteRepairPackagesMessage)", &pLocString);
                    break;
                case BOOTSTRAPPER_ACTION_STATE_MINOR_UPGRADE:
                    LocGetString(m_pWixLoc, L"#(loc.ExecuteUpgradePackagesMessage)", &pLocString);
                    break;
            }

            LPWSTR sczFormattedString = NULL;
            if (pLocString)
            {
                BalFormatString(pLocString->wzText, &sczFormattedString);
            }

            ::SetWindowTextW(m_hwndControlProgressActionText, sczFormattedString ? sczFormattedString : L"Processing packages");
        }
        else
        {
            BalLog(BOOTSTRAPPER_LOG_LEVEL_ERROR, "Don't have progress control!");
        }

        return __super::OnExecutePackageBegin(wzPackageId, fExecute, action, uiLevel, fDisableExternalUiHandler, pfCancel);
    }

    virtual STDMETHODIMP OnExecuteMsiMessage(
        __in_z LPCWSTR wzPackageId,
        __in INSTALLMESSAGE messageType,
        __in DWORD dwUIHint,
        __in_z LPCWSTR wzMessage,
        __in DWORD cData,
        __in_ecount_z_opt(cData) LPCWSTR* rgwzData,
        __in int nRecommendation,
        __inout int* pResult
        )
    {
        if (messageType == INSTALLMESSAGE_ACTIONSTART && cData >= 3)
        {
            if (m_hwndControlProgressActionText)
            {
                // Second field contains human-readable action description
                ::SetWindowTextW(m_hwndControlProgressActionText, rgwzData[1]);
            }
        }

        return __super::OnExecuteMsiMessage(wzPackageId, messageType, dwUIHint, wzMessage, cData, rgwzData, nRecommendation, pResult);
    }

    virtual STDMETHODIMP OnExecuteComplete(
        __in HRESULT hrStatus
        )
    {
        if (m_hwndControlProgressActionText)
        {
            ::SetWindowTextW(m_hwndControlProgressActionText, L"");
        }

        return __super::OnExecuteComplete(hrStatus);
    }

private:
    WIX_LOCALIZATION *m_pWixLoc = NULL;
    HWND m_hwndControlProgressActionText = NULL;
};

static HINSTANCE vhInstance = NULL;

extern "C" BOOL WINAPI DllMain(
    IN HINSTANCE hInstance,
    IN DWORD dwReason,
    IN LPVOID /*pvReserved*/
    )
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        ::DisableThreadLibraryCalls(hInstance);
        vhInstance = hInstance;
        break;

    case DLL_PROCESS_DETACH:
        vhInstance = NULL;
        break;
    }

    return TRUE;
}

extern "C" HRESULT WINAPI BAFunctionsCreate(
    __in const BA_FUNCTIONS_CREATE_ARGS* pArgs,
    __inout BA_FUNCTIONS_CREATE_RESULTS* pResults
    )
{
    HRESULT hr = S_OK;

    IBootstrapperEngine* pEngine = NULL;
    LPWSTR sczModulePath = NULL;
    WCHAR sczLanguage[6] = L"";
    SIZE_T cch = countof(sczLanguage);
    LPWSTR sczLocPath = NULL;
    WIX_LOCALIZATION *pWixLoc = NULL;
    Pencil2DBAFunctions* pBAFunctions = NULL;

    hr = BalInitializeFromCreateArgs(pArgs->pBootstrapperCreateArgs, &pEngine);
    ExitOnFailure(hr, "Failed to initialize Bal.");

    hr = XmlInitialize();
    BalExitOnFailure(hr, "Failed to initialize XML util.");

    hr = PathRelativeToModule(&sczModulePath, NULL, vhInstance);
    BalExitOnFailure(hr, "Failed to get module path.");

    // Language Identifiers are 16 bit, hence max. 5 digits + terminating null character
    hr = pEngine->GetVariableString(L"WixStdBALanguageId", sczLanguage, &cch);
    BalExitOnFailure(hr, "Failed to get language id.");

    hr = LocProbeForFile(sczModulePath, L"thm.wxl", sczLanguage, &sczLocPath);
    BalExitOnFailure(hr, "Failed to probe for loc file: %ls in path: %ls", L"thm.wxl", sczModulePath);

    hr = LocLoadFromFile(sczLocPath, &pWixLoc);
    BalExitOnFailure(hr, "Failed to load loc file from path: %ls", sczLocPath);

    pBAFunctions = new Pencil2DBAFunctions(vhInstance, pEngine, pArgs, pWixLoc);
    BalExitOnNull(pBAFunctions, hr, E_OUTOFMEMORY, "Failed to create new Pencil2DBAFunctions object.");

    pResults->pfnBAFunctionsProc = BalBaseBAFunctionsProc;
    pResults->pvBAFunctionsProcContext = pBAFunctions;
    pBAFunctions = NULL;

LExit:
    ReleaseObject(pBAFunctions);
    ReleaseStr(sczLocPath);
    ReleaseStr(sczModulePath);
    ReleaseObject(pEngine);

    return hr;
}

extern "C" void WINAPI BAFunctionsDestroy(
    __in const BA_FUNCTIONS_DESTROY_ARGS* /*pArgs*/,
    __inout BA_FUNCTIONS_DESTROY_RESULTS* /*pResults*/
    )
{
    XmlUninitialize();
    BalUninitialize();
}

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
#include "verutil.h"
#include "xmlutil.h"

#include "BootstrapperEngineTypes.h"
#include "BootstrapperApplicationTypes.h"
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
        __in HMODULE hModule
        ) : CBalBaseBAFunctions(hModule)
    {
    }

    ~Pencil2DBAFunctions()
    {
        LocFree(m_pWixLoc);
    }

    virtual STDMETHODIMP OnCreate(
        __in IBootstrapperEngine* pEngine,
        __in BOOTSTRAPPER_COMMAND* pCommand
        )
    {
        memcpy_s(&m_command, sizeof(m_command), pCommand, sizeof(BOOTSTRAPPER_COMMAND));

        return __super::OnCreate(pEngine, pCommand);
    }

    virtual STDMETHODIMP OnThemeLoaded(
        __in HWND hWnd
    )
    {
        HRESULT hr = S_OK;
        LPWSTR sczModulePath = NULL;
        LPWSTR sczLanguage = NULL;
        LPWSTR sczLocPath = NULL;

        hr = PathRelativeToModule(&sczModulePath, NULL, m_hModule);
        BalExitOnFailure(hr, "Failed to get module path.");

        hr = BalGetStringVariable(L"WixStdBALanguageId", &sczLanguage);
        BalExitOnFailure(hr, "Failed to get language id.");

        hr = LocProbeForFile(sczModulePath, L"thm.wxl", sczLanguage, &sczLocPath);
        BalExitOnFailure(hr, "Failed to probe for loc file: %ls in path: %ls", L"thm.wxl", sczModulePath);

        hr = LocLoadFromFile(sczLocPath, &m_pWixLoc);
        BalExitOnFailure(hr, "Failed to load loc file from path: %ls", sczLocPath);

        hr = __super::OnThemeLoaded(hWnd);

    LExit:
        ReleaseStr(sczLanguage);
        ReleaseStr(sczLocPath);
        ReleaseStr(sczModulePath);

        return hr;
    }

    virtual STDMETHODIMP OnThemeControlLoading(
        __in LPCWSTR wzName,
        __inout BOOL* pfProcessed,
        __inout WORD* pwId,
        __inout DWORD* pdwAutomaticBehaviorType
        )
    {
        // Take control of BAFunctions-managed controls
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

        // Start marquee effect on progress bars
        WCHAR szClass[countof(PROGRESS_CLASSW)] = L"";
        if (!::GetClassNameW(hWnd, szClass, countof(szClass)))
        {
            BalExitWithLastError(hr, "Failed to get window class.");
        }
        if (::CompareStringW(LOCALE_NEUTRAL, 0, szClass, -1, PROGRESS_CLASSW, -1) == CSTR_EQUAL &&
            ::GetWindowLongPtrW(hWnd, GWL_STYLE) & PBS_MARQUEE)
        {
            ::SendMessageW(hWnd, PBM_SETMARQUEE, TRUE, 0);
        }

        // Store control HWNDs for later
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
        HRESULT hr = S_OK;
        LPWSTR sczVersion = NULL;

        if (relationType == BOOTSTRAPPER_RELATION_UPGRADE)
        {
            BalLog(BOOTSTRAPPER_LOG_LEVEL_STANDARD, "Trying to recover installation options from related bundle %ls.", wzBundleId);
            RecoverRelatedBundleStringVariable(wzBundleId, L"InstallFolder", TRUE);
            RecoverRelatedBundleNumericVariable(wzBundleId, L"DesktopShortcut");

            if (m_command.action == BOOTSTRAPPER_ACTION_INSTALL)
            {
                hr = BalGetVersionVariable(L"WixBundleVersion", &sczVersion);
                BalExitOnFailure(hr, "Failed to get bundle version.");

                int nResult;
                hr = VerCompareStringVersions(wzVersion, sczVersion, TRUE, &nResult);
                BalExitOnFailure(hr, "Failed to compare bundle version: %ls to related bundle version: %ls.", sczVersion, wzVersion);

                if (nResult < 0)
                {
                    BalSetNumericVariable(L"UpgradeDetected", 1);
                }
            }
        }

        hr = __super::OnDetectRelatedBundle(wzBundleId, relationType, wzBundleTag, fPerMachine, wzVersion, fMissingFromCache, pfCancel);

    LExit:
        ReleaseStr(sczVersion);
        return hr;
    }

    virtual STDMETHODIMP OnPauseAutomaticUpdatesBegin()
    {
        SetProgressActionText(L"#(loc.PauseAutomaticUpdatesMessage)", L"Pausing Windows automatic updates");

        return __super::OnPauseAutomaticUpdatesBegin();
    }

    virtual STDMETHODIMP OnSystemRestorePointBegin()
    {
        SetProgressActionText(L"#(loc.SystemRestorePointMessage)", L"Creating system restore point");

        return __super::OnSystemRestorePointBegin();
    }

    virtual STDMETHODIMP OnCacheBegin(
        __inout BOOL* pfCancel
        )
    {
        SetProgressActionText(L"#(loc.CacheMessage)", L"Preparing files");

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
        switch (action) {
            case BOOTSTRAPPER_ACTION_STATE_UNINSTALL:
                SetProgressActionText(L"#(loc.ExecuteUninstallPackagesMessage)", L"Uninstalling packages");
                break;
            case BOOTSTRAPPER_ACTION_STATE_INSTALL:
                SetProgressActionText(L"#(loc.ExecuteInstallPackagesMessage)", L"Installing packages");
                break;
            case BOOTSTRAPPER_ACTION_STATE_MODIFY:
                SetProgressActionText(L"#(loc.ExecuteModifyPackagesMessage)", L"Modifying packages");
                break;
            case BOOTSTRAPPER_ACTION_STATE_REPAIR:
                SetProgressActionText(L"#(loc.ExecuteRepairPackagesMessage)", L"Repairing packages");
                break;
            case BOOTSTRAPPER_ACTION_STATE_MINOR_UPGRADE:
                SetProgressActionText(L"#(loc.ExecuteUpgradePackagesMessage)", L"Upgrading packages");
                break;
            default:
                BalLog(BOOTSTRAPPER_LOG_LEVEL_ERROR, "Unknown action state %d", action);
                // Should never happen anyway, but at least make sure progress text isn't completely wrong
                SetProgressActionText(NULL, L"Processing packages");
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
        if (messageType == INSTALLMESSAGE_ACTIONSTART && cData >= 2)
        {
            // Second field contains human-readable action description
            SetProgressActionText(NULL, rgwzData[1]);
        }

        return __super::OnExecuteMsiMessage(wzPackageId, messageType, dwUIHint, wzMessage, cData, rgwzData, nRecommendation, pResult);
    }

    virtual STDMETHODIMP OnExecuteComplete(
        __in HRESULT hrStatus
        )
    {
        SetProgressActionText(NULL, L"");

        return __super::OnExecuteComplete(hrStatus);
    }

private:
    HRESULT RecoverRelatedBundleStringVariable(
        __in_z LPCWSTR wzBundleId,
        __in_z LPCWSTR wzVariable,
        __in BOOL fFormatted
        )
    {
        HRESULT hr = S_OK;
        LPWSTR wzValue = NULL;

        hr = BalGetRelatedBundleVariable(wzBundleId, wzVariable, &wzValue);
        BalExitOnFailure(hr, "Failed to get variable %ls from related bundle %ls.", wzVariable, wzBundleId);

        if (wzValue)
        {
            hr = BalSetStringVariable(wzVariable, wzValue, fFormatted);
            BalExitOnFailure(hr, "Failed to set variable %ls to recovered value %ls.", wzVariable, wzValue);
        }

    LExit:
        ReleaseStr(wzValue);
        return hr;
    }

    HRESULT RecoverRelatedBundleNumericVariable(
        __in_z LPCWSTR wzBundleId,
        __in_z LPCWSTR wzVariable
        )
    {
        HRESULT hr = S_OK;
        LPWSTR wzValue = NULL;
        LONGLONG llValue = 0;

        hr = BalGetRelatedBundleVariable(wzBundleId, wzVariable, &wzValue);
        BalExitOnFailure(hr, "Failed to get variable %ls from related bundle %ls.", wzVariable, wzBundleId);

        hr = StrStringToInt64(wzValue, 0, &llValue);
        BalExitOnFailure(hr, "Failed to convert value %ls of variable %ls recovered from related bundle to number.", wzValue, wzVariable);

        hr = BalSetNumericVariable(wzVariable, llValue);
        BalExitOnFailure(hr, "Failed to set variable %ls to recovered value %lld.", wzVariable, llValue);

    LExit:
        ReleaseStr(wzValue);
        return hr;
    }

    void SetProgressActionText(
        __in_z LPCWSTR wzLocId,
        __in_z LPCWSTR wzFallbackText
        )
    {
        if (!m_hwndControlProgressActionText)
        {
            return;
        }

        LOC_STRING* pLocString = NULL;
        LPWSTR sczFormattedString = NULL;
        LocGetString(m_pWixLoc, wzLocId, &pLocString);
        if (pLocString)
        {
            BalFormatString(pLocString->wzText, &sczFormattedString);
        }

        ::SetWindowTextW(m_hwndControlProgressActionText, sczFormattedString ? sczFormattedString : wzFallbackText);

        ReleaseStr(sczFormattedString);
    }

    WIX_LOCALIZATION *m_pWixLoc = NULL;
    BOOTSTRAPPER_COMMAND m_command;
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
    Pencil2DBAFunctions* pBAFunctions = NULL;

    BalInitialize(pArgs->pEngine);

    hr = XmlInitialize();
    BalExitOnFailure(hr, "Failed to initialize XML util.");

    pBAFunctions = new Pencil2DBAFunctions(vhInstance);
    BalExitOnNull(pBAFunctions, hr, E_OUTOFMEMORY, "Failed to create new Pencil2DBAFunctions object.");

    hr = pBAFunctions->OnCreate(pArgs->pEngine, pArgs->pCommand);
    ExitOnFailure(hr, "Failed to call OnCreate Pencil2DBAFunctions.");

    pResults->pfnBAFunctionsProc = BalBaseBAFunctionsProc;
    pResults->pvBAFunctionsProcContext = pBAFunctions;
    pBAFunctions = NULL;

LExit:
    ReleaseObject(pBAFunctions);

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

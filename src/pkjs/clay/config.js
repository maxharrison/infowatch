var meta = require('../../../package.json');
var versionLabel = "v" + meta.version;

module.exports = [
    {
        "type": "heading",
        "defaultValue": "ForecasWatch2"
    },
    {
        "type": "text",
        "defaultValue": "Contribute on <a href=\"https://github.com/mattrossman/forecaswatch2\">GitHub!</a>"
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Second city time"
            },
            {
                "type": "text",
                "defaultValue": "Show a second city's time in a small line below the main clock. The offset is a fixed UTC offset (it does not adjust for daylight saving time)."
            },
            {
                "type": "toggle",
                "label": "Show second city",
                "messageKey": "secondCityEnabled",
                "defaultValue": true
            },
            {
                "type": "input",
                "label": "City label",
                "messageKey": "secondCityLabel",
                "defaultValue": "BKK",
                "description": "A short label shown before the time, e.g. \"BKK\". Leave blank to show just the time.",
                "attributes": {
                    "placeholder": "e.g. BKK",
                    "maxlength": 7
                }
            },
            {
                "type": "select",
                "label": "Timezone (UTC offset)",
                "messageKey": "secondCityOffset",
                "defaultValue": "420",
                "options": [
                    { "label": "UTC-12:00", "value": "-720" },
                    { "label": "UTC-11:00", "value": "-660" },
                    { "label": "UTC-10:00", "value": "-600" },
                    { "label": "UTC-09:30", "value": "-570" },
                    { "label": "UTC-09:00", "value": "-540" },
                    { "label": "UTC-08:00", "value": "-480" },
                    { "label": "UTC-07:00", "value": "-420" },
                    { "label": "UTC-06:00", "value": "-360" },
                    { "label": "UTC-05:00", "value": "-300" },
                    { "label": "UTC-04:00", "value": "-240" },
                    { "label": "UTC-03:30", "value": "-210" },
                    { "label": "UTC-03:00", "value": "-180" },
                    { "label": "UTC-02:00", "value": "-120" },
                    { "label": "UTC-01:00", "value": "-60" },
                    { "label": "UTC+00:00", "value": "0" },
                    { "label": "UTC+01:00", "value": "60" },
                    { "label": "UTC+02:00", "value": "120" },
                    { "label": "UTC+03:00", "value": "180" },
                    { "label": "UTC+03:30", "value": "210" },
                    { "label": "UTC+04:00", "value": "240" },
                    { "label": "UTC+04:30", "value": "270" },
                    { "label": "UTC+05:00", "value": "300" },
                    { "label": "UTC+05:30", "value": "330" },
                    { "label": "UTC+05:45", "value": "345" },
                    { "label": "UTC+06:00", "value": "360" },
                    { "label": "UTC+06:30", "value": "390" },
                    { "label": "UTC+07:00 (Bangkok)", "value": "420" },
                    { "label": "UTC+08:00", "value": "480" },
                    { "label": "UTC+08:45", "value": "525" },
                    { "label": "UTC+09:00", "value": "540" },
                    { "label": "UTC+09:30", "value": "570" },
                    { "label": "UTC+10:00", "value": "600" },
                    { "label": "UTC+10:30", "value": "630" },
                    { "label": "UTC+11:00", "value": "660" },
                    { "label": "UTC+12:00", "value": "720" },
                    { "label": "UTC+12:45", "value": "765" },
                    { "label": "UTC+13:00", "value": "780" },
                    { "label": "UTC+14:00", "value": "840" }
                ]
            }
        ]
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Weather"
            },
            {
                "type": "toggle",
                "label": "Day/night shading",
                "messageKey": "dayNightShading",
                "defaultValue": true,
                "description": "Show hatch shading between sunset and sunrise to distinguish day and night on the forecast graph."
            },
            {
                "type": "radiogroup",
                "label": "Provider",
                "messageKey": "provider",
                "defaultValue": "wunderground",
                "options": [
                    {
                        "label": "Weather Underground",
                        "value": "wunderground"
                    },
                    {
                        "label": "OpenWeatherMap",
                        "value": "openweathermap"
                    }
                ]
            },
            {
                "type": "input",
                "label": "OpenWeatherMap API key",
                "messageKey": "owmApiKey",
                "description": "<a href='https://openweathermap.org/'>Register an OpenWeatherMap account</a> and paste your API key here"
            },
            {
                "type": "toggle",
                "label": "Force weather fetch",
                "messageKey": "fetch",
                "description": "Last successful fetch:<br><span id='lastFetchSpan'>Never :(</span><span id='lastAttemptBlock'></span>"
            },
            {
                "type": "input",
                "label": "Location override",
                "messageKey": "location",
                "description": "Example: \"Manhattan\" or \"123 Oak St Plainsville KY\".<br><a href=\"https://locationiq.com/demo\">Click here</a> to test out your location query.<br>To use GPS, leave this blank and ensure GPS is enabled on your device.",
                "attributes": {
                    "placeholder": "Using GPS",
                }
            }
        ]
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Misc"
            },
            {
                "type": "toggle",
                "label": "Show quiet time icon",
                "messageKey": "showQt",
                "defaultValue": true
            },
            {
                "type": "toggle",
                "label": "Vibrate on bluetooth disconnect",
                "messageKey": "vibe",
                "defaultValue": false
            },
            {
                "type": "select",
                "defaultValue": "both",
                "messageKey": "btIcons",
                "label": "Show icon for bluetooth",
                "options": [
                    {
                        "label": "Disconnected",
                        "value": "disconnected"
                    },
                    {
                        "label": "Connected",
                        "value": "connected"
                    },
                    {
                        "label": "Both",
                        "value": "both"
                    },
                    {
                        "label": "None",
                        "value": "none"
                    }
                ]
            },
        ]
    },
    {
        "type": "submit",
        "defaultValue": "Save Settings"
    },
    {
        "type": "text",
        "defaultValue": versionLabel
    }
]

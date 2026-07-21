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
                "defaultValue": "Time",
            },
            {
                "type": "toggle",
                "label": "Leading zero",
                "messageKey": "timeLeadingZero",
            },
            {
                "type": "toggle",
                "label": "Show AM/PM",
                "messageKey": "timeShowAmPm",
            },
            {
                "type": "select",
                "label": "Main time font",
                "messageKey": "timeFont",
                "defaultValue": "roboto",
                "options": [
                    {
                        "label": "Roboto",
                        "value": "roboto"
                    },
                    {
                        "label": "Leco",
                        "value": "leco"
                    },
                    {
                        "label": "Bitham",
                        "value": "bitham"
                    },
                ]
            },
            {
                "type": "color",
                "label": "Main time color",
                "messageKey": "colorTime",
                "defaultValue": "#FFFFFF",
                "sunlight": false,
                "capabilities": ["COLOR"]
            },
        ]
    },
    {
        "type": "section",
        "items": [
            {
                "type": "heading",
                "defaultValue": "Calendar",
            },
            {
                "type": "select",
                "label": "First week to display",
                "messageKey": "firstWeek",
                "defaultValue": "prev",
                "options": [
                    {
                        "label": "Previous week",
                        "value": "prev"
                    },
                    {
                        "label": "Current week",
                        "value": "curr"
                    }
                ]
            },
            {
                "type": "color",
                "label": "Today highlight",
                "messageKey": "colorToday",
                "defaultValue": "#000000",
                "description": "Black (default) means match date color, any other value overrides this.",
                "sunlight": false,
                "capabilities": ["COLOR"]
            },
            {
                "type": "color",
                "label": "Sunday color",
                "messageKey": "colorSunday",
                "defaultValue": "#FF0000",
                "sunlight": false,
                "capabilities": ["COLOR"]
            },
            {
                "type": "color",
                "label": "Saturday color",
                "messageKey": "colorSaturday",
                "defaultValue": "#FF5500",
                "sunlight": false,
                "capabilities": ["COLOR"]
            },
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

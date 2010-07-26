if (!window.dfx) {
    function dfx() {}
}

dfx.date = function(format, timestamp, tsIso8601)
{
    if (timestamp === null && tsIso8601) {
        timestamp = dfx.tsIso8601ToTimestamp(tsIso8601);
        if (!timestamp) {
            return;
        }
    }

    var date    = new Date(timestamp);
    var formats = format.split('');
    var fc      = formats.length;
    var dateStr = '';

    for (var i = 0; i < fc; i++) {
        var r = '';
        var f = formats[i];
        switch (f) {
            case 'D':
            case 'l':
                var names = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
                r         = names[date.getDay()];
                if (f === 'D') {
                    r = r.substring(0, 3);
                }
            break;

            case 'F':
            case 'M':
                months = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];
                r      = months[date.getMonth()];
                if (f === 'M') {
                    r = r.substring(0, 3);
                }
            break;

            case 'd':
                r = date.getDate();
            break;

            case 'S':
                r = dfx.getOrdinalSuffix(date.getDate());
            break;

            case 'Y':
            case 'y':
                r = date.getFullYear();
                if (f === 'y') {
                    r = r.toString().substring(2);
                }
            break;

            case 'H':
                r = date.getHours();
            break;

            case 'h':
                r = date.getHours();
                if (r === 0) {
                    r = 12;
                } else if (r > 12) {
                    r -= 12;
                }
            break;

            case 'i':
                r = dfx.addNumberPadding(date.getMinutes());
            break;

            case 'a':
                r = 'am';
                if (date.getHours() >= 12) {
                    r = 'pm';
                }
            break;

            default:
                r = f;
            break;
        }//end switch

        // Append the replacement to our date string.
        dateStr += r;
    }//end for

    return dateStr;

};

dfx.getOrdinalSuffix = function(number)
{
    var suffix = '';
    var tmp    = (number % 100);

    if (tmp >= 4 && tmp <= 20) {
        suffix = 'th';
    } else {
        switch (number % 10) {
            case 1:
                suffix = 'st';
            break;

            case 2:
                suffix = 'nd';
            break;

            case 3:
                suffix = 'rd';
            break;

            default:
                suffix = 'th';
            break;
        }
    }//end if

    return suffix;

};

dfx.addNumberPadding = function(number)
{
    if (number < 10) {
        number = '0' + number;
    }

    return number;

};

dfx.tsIso8601ToTimestamp = function(tsIso8601)
{
    var regexp = /(\d\d\d\d)(?:-?(\d\d)(?:-?(\d\d)(?:[T ](\d\d)(?::?(\d\d)(?::?(\d\d)(?:\.(\d+))?)?)?(?:Z|(?:([-+])(\d\d)(?::?(\d\d))?)?)?)?)?)?/;
    var d      = tsIso8601.match(new RegExp(regexp));

    if (d) {
        var date = new Date();
        date.setDate(d[3]);
        date.setFullYear(d[1]);
        date.setMonth(d[2] - 1);
        date.setHours(d[4]);
        date.setMinutes(d[5]);
        date.setSeconds(d[6]);

        var offset = (d[9] * 60);

        if (d[8] === '+') {
            offset *= -1;
        }

        offset       -= date.getTimezoneOffset();
        var timestamp = (date.getTime() + (offset * 60 * 1000));
        return timestamp;
    }

    return null;

};

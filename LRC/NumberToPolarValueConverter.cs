using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Data;
using System.Globalization;

namespace LRC
{
    [ValueConversion(typeof(object), typeof(int))]
    public class NumberToPolarValueConverter : IValueConverter
    {
        private static int i = -2;
        public object Convert(
            object value, Type targetType,
            object parameter, CultureInfo culture)
        {
            i++;
            if (i >= 2)
            {
                i = -1;
            }
            return i;
        }

        public object ConvertBack(
            object value, Type targetType,
            object parameter, CultureInfo culture)
        {
            throw new NotSupportedException("ConvertBack not supported");
        }
    }
}

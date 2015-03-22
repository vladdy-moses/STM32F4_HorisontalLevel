using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;
using System.Windows.Media;

namespace HorisontLevelClient
{
    public static class CanvasExtensions
    {
        public static Canvas SetCoordinateSystem(this Canvas canvas, Double xMin, Double xMax, Double yMin, Double yMax)
        {
            var width = xMax - xMin;
            var height = yMax - yMin;

            var translateX = -xMin;
            var translateY = height + yMin;

            var group = new TransformGroup();

            group.Children.Add(new TranslateTransform(translateX, -translateY));
            group.Children.Add(new ScaleTransform(canvas.ActualWidth / width, canvas.ActualHeight / -height));

            canvas.RenderTransform = group;

            return canvas;
        }
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace wpfdemo
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private CartoType.Framework m_framework;
        private CartoType.MapRenderer m_map_renderer;
        private bool m_graphics_acceleration = false;
        private bool m_map_drag_enabled;
        private int m_map_drag_offset_x;
        private int m_map_drag_offset_y;
        private int m_map_drag_anchor_x;
        private int m_map_drag_anchor_y;

        public MainWindow()
        {
            InitializeComponent();
            String font_path = "g:/cartotype/font/";

            m_framework = new CartoType.Framework("g:/cartotype/map/corsica.ctm1",
                                                  "g:/cartotype/style/standard.ctstyle",
                                                  font_path + "DejaVuSans.ttf",
                                                  256,
                                                  256);
            m_framework.LoadFont(font_path + "DejaVuSans-Bold.ttf");
            m_framework.LoadFont(font_path + "DejaVuSerif.ttf");
            m_framework.LoadFont(font_path + "DejaVuSerif-Italic.ttf");
            m_framework.SetResolutionDpi(144);
            m_framework.SetFollowMode(CartoType.FollowMode.LocationHeading);

        }

        protected void Window_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (!e.NewSize.IsEmpty && m_framework != null)
            {
                if (m_map_renderer == null)
                {
                    m_map_renderer = new CartoType.MapRenderer(m_framework, new WindowInteropHelper(this).Handle);
                    m_graphics_acceleration = m_map_renderer.Valid();
                }

                m_framework.Resize((int)e.NewSize.Width, (int)e.NewSize.Height);
            }
        }

        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
            {
                m_map_drag_enabled = true;
                m_map_drag_anchor_x = (int)e.GetPosition(Application.Current.MainWindow).X;
                m_map_drag_anchor_y = (int)e.GetPosition(Application.Current.MainWindow).Y;
            }

        }

        private void Window_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (e.ChangedButton == MouseButton.Left)
            {
                m_map_drag_enabled = false;
                m_map_drag_offset_x = 0;
                m_map_drag_offset_y = 0;
            }
        }

        private void Window_MouseMove(object sender, MouseEventArgs e)
        {
            if (m_map_drag_enabled)
            {
                Point p = e.GetPosition(Application.Current.MainWindow);
                m_map_drag_offset_x = (int)p.X - m_map_drag_anchor_x;
                m_map_drag_offset_y = (int)p.Y - m_map_drag_anchor_y;
                m_framework.Pan(-m_map_drag_offset_x, -m_map_drag_offset_y);
                m_map_drag_anchor_x = (int)p.X;
                m_map_drag_anchor_y = (int)p.Y;
                m_map_drag_offset_x = 0;
                m_map_drag_offset_y = 0;
            }
        }

        private void Window_MouseWheel(object sender, MouseWheelEventArgs e)
        {
            int zoom_count = e.Delta / 120;
            double zoom = Math.Sqrt(2);
            if (zoom_count == 0)
                zoom_count = e.Delta >= 0 ? 1 : -1;
            zoom = Math.Pow(zoom, zoom_count);

            Point p = e.GetPosition(Application.Current.MainWindow);
            if (p.X < ActualWidth && p.Y < ActualHeight)
                m_framework.ZoomAt(zoom, (int)p.X, (int)p.Y, CartoType.CoordType.Screen);
            else
                m_framework.Zoom(zoom);

        }
    }

}




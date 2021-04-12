using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WebControlLED
{
    public partial class Form1 : Form
    {
        static string ConfigKeyAddress = "ADDRESS";
        static string ConfigKeyR = "COLOR_R";
        static string ConfigKeyG = "COLOR_G";
        static string ConfigKeyB = "COLOR_B";
        static string ConfigKeyBright = "BRIGHT";


        LEDControl control = null;
        bool led_switch = false, led_mode = false;
        int r = 0, g = 0, b = 0, bright = 100;
        public Form1()
        {
            InitializeComponent();
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            string address = AppConfigAccess.GetAppSetting(ConfigKeyAddress);
            r = Convert.ToInt32(AppConfigAccess.GetAppSetting(ConfigKeyR));
            g = Convert.ToInt32(AppConfigAccess.GetAppSetting(ConfigKeyG));
            b = Convert.ToInt32(AppConfigAccess.GetAppSetting(ConfigKeyB));
            bright = Convert.ToInt32(AppConfigAccess.GetAppSetting(ConfigKeyBright));

            trackBar1.Value = r;
            trackBar2.Value = g;
            trackBar3.Value = b;
            trackBar4.Value = bright;

            textBox_R.Text = r.ToString();
            textBox_G.Text = g.ToString();
            textBox_B.Text = b.ToString();
            textBox_bright.Text = bright.ToString();


            button_switch.Text = "关";
            button_mode.Text = "关";
            

            label_color_change();
            control = new LEDControl(address);
            textBox_address.Text = address;
        }

        private void button_switch_Click(object sender, EventArgs e)
        {
            control.SetSwitch(led_switch);
            led_switch = !led_switch;
            if(led_switch)
            {
                button_switch.Text = "开";
            }
            else
            {
                button_switch.Text = "关";
            }
        }

        private void trackBar2_Scroll(object sender, EventArgs e)
        {
            g = trackBar2.Value;
            textBox_G.Text = g.ToString();
            label_color_change();
        }

        private void trackBar3_Scroll(object sender, EventArgs e)
        {
            b = trackBar3.Value;
            textBox_B.Text = b.ToString();
            label_color_change();
        }

        private void trackBar4_Scroll(object sender, EventArgs e)
        {
            bright = trackBar4.Value;
            textBox_bright.Text = bright.ToString();
        }

        private void textBox_R_TextChanged(object sender, EventArgs e)
        {
            if (textBox_R.Text != "")
            {
                int i = Convert.ToInt32(textBox_R.Text);
                if (i <= 255 && i >= 0)
                { trackBar1.Value = Convert.ToInt32(textBox_R.Text); }
            }
        }

        private void textBox_G_TextChanged(object sender, EventArgs e)
        {
            if (textBox_G.Text != "")
            {
                int i = Convert.ToInt32(textBox_G.Text);
                if (i <= 255 && i >= 0)
                { trackBar2.Value = Convert.ToInt32(textBox_G.Text); }
            }
        }

        private void textBox_B_TextChanged(object sender, EventArgs e)
        {
            if (textBox_G.Text != "")
            {
                int i = Convert.ToInt32(textBox_B.Text);
                if (i <= 255 && i >= 0)
                { trackBar3.Value = Convert.ToInt32(textBox_B.Text); }
            }
        }

        private void textBox_bright_TextChanged(object sender, EventArgs e)
        {
            if (textBox_bright.Text != "")
            {
                int i = Convert.ToInt32(textBox_bright.Text);
                if (i <= 255 && i >= 0)
                { trackBar4.Value = Convert.ToInt32(textBox_bright.Text); }
            }
        }

        private void textBox_R_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar != 8 && !Char.IsDigit(e.KeyChar))
            {
                e.Handled = true;
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(Regex.IsMatch(textBox_address.Text, @"^((2(5[0-5]|[0-4]\d))|[0-1]?\d{1,2})(\.((2(5[0-5]|[0-4]\d))|[0-1]?\d{1,2})){3}$"))
            {
                AppConfigAccess.UpdateAppSettings(ConfigKeyAddress, textBox_address.Text);
            }
            AppConfigAccess.UpdateAppSettings(ConfigKeyR, trackBar1.Value.ToString());
            AppConfigAccess.UpdateAppSettings(ConfigKeyG, trackBar2.Value.ToString());
            AppConfigAccess.UpdateAppSettings(ConfigKeyB, trackBar3.Value.ToString());
            AppConfigAccess.UpdateAppSettings(ConfigKeyBright, trackBar4.Value.ToString());


        }

        private void button_refersh_Click(object sender, EventArgs e)
        {
            control.SetColor(r, g, b);
            control.SetBright(bright);
        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            r = trackBar1.Value;
            textBox_R.Text = r.ToString();
            label_color_change();
        }
        private void label_color_change()
        {
            label_color.BackColor = Color.FromArgb(r, g, b);
        }

        private void button_mode_Click(object sender, EventArgs e)
        {
            control.SetMode(led_mode);
            led_mode = !led_mode;
            if (led_mode)
            {
                button_mode.Text = "开";
            }
            else
            {
                button_mode.Text = "关";
            }
        }
    }
}

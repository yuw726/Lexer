using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Kurs
{
    public class OptionsForm : Form
    {
        public OptionsForm ()
        {
            InitializeComponent();
            MessageBox.Show("Heheyy!", this.ToString());
        }

        public int thickness
        {
            get
            {
                return comboBox1.SelectedIndex; // This is comment
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
            this.Dispose(); /* This is also comment */
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.Close();
            this.Dispose();
        }
    }
}

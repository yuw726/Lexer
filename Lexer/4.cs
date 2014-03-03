using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Kurs
{
    public class MainForm : Form
    {        
        AboutForm newAbout;
        TypeForm newType;        
        object cellVal;
        DirectoryInfo programDirectory = new DirectoryInfo(".");

        public MainForm()
        {
            InitializeComponent();
            dataGridView1.RowCount = 100;             
            dataGridView1.Height = Screen.PrimaryScreen.WorkingArea.Height - 100;
            dataGridView1.Width = Screen.PrimaryScreen.WorkingArea.Width;            
        }

        private void MainForm_KeyUp(object sender, KeyEventArgs e)
        {
            if ((e.Control == true) && (e.KeyCode == Keys.C))
            {
                cellVal = dataGridView1.CurrentCell.Value;
            }
            if ((e.Control == true) && (e.KeyCode == Keys.X))
            {
                cellVal = dataGridView1.CurrentCell.Value;                
                dataGridView1.CurrentCell.Value = null;
            }
            if ((e.Control == true) && (e.KeyCode == Keys.V))
            {
                if (cellVal != null)
                    dataGridView1.CurrentCell.Value = cellVal;
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (File.Exists(programDirectory.FullName.ToString() + "//temp_datum.elf"))
            {
                File.Delete(programDirectory.FullName.ToString() + "//temp_datum.elf");
            }
        }

        // ----------------------------------------------------------------
        // Menu & Panel Interface

        private void openToolStripButton_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel2.Text = "Идёт открытие файла...";
            if (DialogResult.OK == openFileDialog1.ShowDialog())
            {
                newToolStripButton.PerformClick(); 
                string[] cell_to_string = new string[100];
                int i = 0, j = 0, k = 0;
                foreach (string row_data in File.ReadAllLines(openFileDialog1.FileName))
                {
                    foreach (char cell_data in row_data)
                    {
                        if (cell_data != ' ')
                        {
                            cell_to_string[k] = cell_to_string[k] + cell_data;
                        }
                        else
                        {
                            dataGridView1.Rows[i].Cells[j].Value = cell_to_string[k];
                            ++j;
                            ++k;
                        }
                    }
                    ++i;
                    j = 0;
                }
                toolStripStatusLabel2.Text = "Открыт файл: " + openFileDialog1.FileName;
            }
            else
            {
                toolStripStatusLabel2.Text = "";
            }
        }

        private void saveToolStripButton_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel2.Text = "Идёт сохранение файла...";
            if (DialogResult.OK == saveFileDialog1.ShowDialog())
            {
                string[] cell_to_string = new string[100];
                for (int i = 0; i < dataGridView1.Rows.Count; i++)
                {
                    for (int j = 0; j < dataGridView1.Rows[i].Cells.Count; j++)
                    {
                        if (dataGridView1.Rows[i].Cells[j].Value == null) continue;
                        cell_to_string[i] = cell_to_string[i] + dataGridView1.Rows[i].Cells[j].Value.ToString() + " ";
                    }
                }
                File.WriteAllLines(saveFileDialog1.FileName, cell_to_string);
                toolStripStatusLabel2.Text = "Файл сохранён: " + saveFileDialog1.FileName;
            }
            else
            {
                toolStripStatusLabel2.Text = "";
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openToolStripButton.PerformClick();
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveToolStripButton.PerformClick();
        }

        private void fontToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (DialogResult.OK == fontDialog1.ShowDialog())
            {
                dataGridView1.DefaultCellStyle.Font = fontDialog1.Font;
                dataGridView1.CurrentCell.Style.Font = fontDialog1.Font;
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void шрифтToolStripMenuItem_Click(object sender, EventArgs e)
        {
            fontToolStripMenuItem.PerformClick();
        }

        private void построитьДиаграммуToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStripButton1.PerformClick();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (newAbout == null)
            {
                newAbout = new AboutForm();
                newAbout.Show();
            }
            else
            {
                newAbout.Close();
                newAbout.Dispose();
                newAbout = new AboutForm();
                newAbout.Show();
            }
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel2.Text = "Построение диаграммы";
            string[] cell_to_string = new string[100];
            for (int i = 0; i < dataGridView1.Rows.Count; i++)
            {
                for (int j = 0; j < dataGridView1.Rows[i].Cells.Count; j++)
                {
                    if (dataGridView1.Rows[i].Cells[j].Value == null) continue;
                    cell_to_string[i] = cell_to_string[i] + dataGridView1.Rows[i].Cells[j].Value.ToString() + " ";
                }
            }
            File.WriteAllLines(programDirectory.FullName.ToString() + "//temp_datum.elf", cell_to_string);
            
            if (newType == null)
            {
                newType = new TypeForm();
                newType.ShowDialog();
            }
            else
            {
                newType.Close();
                newType.Dispose();
                newType = new TypeForm();
                newType.ShowDialog();
            }
        }

        private void newToolStripButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < dataGridView1.Rows.Count; i++)
            {
                for (int j = 0; j < dataGridView1.Rows[i].Cells.Count; j++)
                {
                    dataGridView1.Rows[i].Cells[j].Value = null;
                }
            }
            toolStripStatusLabel2.Text = "Таблица очищена";
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < dataGridView1.Rows.Count; i++)
            {
                for (int j = 0; j < dataGridView1.Rows[i].Cells.Count; j++)
                {
                    dataGridView1.Rows[i].Cells[j].Value = null;
                }
            }
            toolStripStatusLabel2.Text = "Таблица очищена";
        }

        private void copyToolStripButton_Click(object sender, EventArgs e)
        {
            cellVal = dataGridView1.CurrentCell.Value;
        }

        private void pasteToolStripButton_Click(object sender, EventArgs e)
        {
            if (cellVal != null)
            {
                dataGridView1.CurrentCell.Value = cellVal;
            }            
        }
        
        private void cutToolStripButton_Click(object sender, EventArgs e)
        {
            cellVal = dataGridView1.CurrentCell.Value;
            dataGridView1.CurrentCell.Value = null;
        }        

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStripStatusLabel2.Text = "Идёт сохранение файла...";
            if (File.Exists(saveFileDialog1.FileName))
            {
                string[] cell_to_string = new string[100];
                for (int i = 0; i < dataGridView1.Rows.Count; i++)
                {
                    for (int j = 0; j < dataGridView1.Rows[i].Cells.Count; j++)
                    {
                        if (dataGridView1.Rows[i].Cells[j].Value == null) continue;
                        cell_to_string[i] = cell_to_string[i] + dataGridView1.Rows[i].Cells[j].Value.ToString() + " ";
                    }
                }
                File.WriteAllLines(saveFileDialog1.FileName, cell_to_string);
                toolStripStatusLabel2.Text = "Файл сохранён: " + saveFileDialog1.FileName;
            }
            else
            {
                saveAsToolStripMenuItem.PerformClick();
                toolStripStatusLabel2.Text = "Файл сохранён";
            }
        }
    }
}
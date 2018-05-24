using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Bane
{
	public class TestClass
	{
		public TestForm1 TestForm;

		public void InitializeForm()
		{
			TestForm = new TestForm1();
			TestForm.Activate();
			TestForm.Show();
			System.Windows.Forms.Application.Run();
			Type ThisType = GetType();
		}
	}
}

class WRC1Array {
	int[] x = {0,0};
	int[] tmp1 = {0,0};
	int[] tmp2 = {0,0};

	class Thread1 extends Thread {
		public void run(){
			x[0] = 1;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmp1[0] = x[0];
			x[1] = 1;
		}
	}

	class Thread3 extends Thread {
		public void run(){
			tmp2[1] = x[1];
			tmp2[0] = x[0];
		}
	}


	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();
		Thread3 t3 = new Thread3();

		t1.start();
		t2.start();
		t3.start();

		try {
			t1.join();
			t2.join();
			t3.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			WRC1Array wrc = new WRC1Array();
			wrc.test();
			if(wrc.tmp1[0] == 1 && wrc.tmp2[1] == 1 && wrc.tmp2[0] == 0) 
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM WRC Test PASSED after 100000 iterations");
		else
			System.out.println("ARM WRC Test FAILED: problem occurs on iteration " + counter);
	}
}

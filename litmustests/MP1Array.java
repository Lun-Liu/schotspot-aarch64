class MP1Array {
	int[] x = {0,0};
	int[] tmp = {0,0};

	class Thread1 extends Thread {
		public void run(){
			x[0] = 1;
			x[1] = 1;
		}
	}

	class Thread2 extends Thread {
		public void run(){
			tmp[1] = x[1];
			tmp[0] = x[0];
		}
	}

	public void test(){
		Thread1 t1 = new Thread1();
		Thread2 t2 = new Thread2();
		t1.start();
		t2.start();

		try {
			t1.join();
			t2.join();
		} catch(InterruptedException e) {
			System.out.println(e);
		}
	}


	public static void main(String[] args){
		int counter = 1;
		while (counter <= 100000){
			MP1Array mp = new MP1Array();
			mp.test();
			if(mp.tmp[1] == 1 && mp.tmp[0] == 0)
				break;
			counter++;
		}

		if(counter == 100001)
			System.out.println("ARM MP Test PASSED after 100000 iterations");
		else
			System.out.println("ARM MP Test FAILED: problem occurs on iteration " + counter);
	}
}
